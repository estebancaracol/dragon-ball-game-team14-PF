#include "Fisica.h"
#include "Goku.h"
#include "Plataforma.h"
#include "Obstaculo.h"
#include "Nivel2.h"

#include <QtMath>
#include <QDebug>
#include <QGraphicsRectItem>
#include <QGraphicsScene>

// ============================================
// CONSTRUCTOR
// ============================================

Fisica::Fisica() {}

// ============================================
// FÍSICA BÁSICA - GRAVEDAD Y COLISIONES
// ============================================

void Fisica::aplicarGravedad(Personaje* personaje)
{
    if (!personaje) return;

    float nuevaY = personaje->y() + 5.0;  // Caída por gravedad base
    personaje->setY(nuevaY);
    // En un mundo real, aquí usarías delta time y aceleración
}

bool Fisica::verificarColisionSuelo(Personaje* personaje, Plataforma* plataforma)
{
    if (!personaje || !plataforma) return false;

    return personaje->collidesWithItem(plataforma);
}

void Fisica::afectarObjeto(ObjetoFisico* obs) {
    if (!obs) return;

    float gravedad = 2.0f;
    obs->setY(obs->y() + gravedad);

    //qDebug() << "[Fisica] Escombro en Y:" << obs->y();
}

// ============================================
// CÁLCULOS DE TRAYECTORIA
// ============================================

QPointF Fisica::calcularTiroParabolico(float angulo, float velocidad)
{
    float rad = qDegreesToRadians(angulo);
    float vx = velocidad * qCos(rad);
    float vy = -velocidad * qSin(rad);
    return QPointF(vx, vy);
}

QPointF Fisica::calcularTiroRecto(float velocidad)
{
    return QPointF(velocidad, 0.0f);
}

// ============================================
// SISTEMA DE PROYECTILES
// ============================================

void Fisica::lanzarProyectil(ObjetoFisico* objeto, const QPointF& velocidad, bool parabolico)
{
    QTimer* timer = new QTimer();
    objeto->setVelocidad(velocidad.x(), velocidad.y());
    objeto->setUsarGravedad(parabolico);
    objeto->setTimer(timer);

    QObject::connect(timer, &QTimer::timeout, [objeto]() {
        if (!objeto || objeto->estaDestruido()) return;
        objeto->actualizarFisica();
    });

    timer->start(30);
}

// ============================================
// EFECTOS ESPECIALES - CAPARAZÓN
// ============================================

void Fisica::aplicarResistencia(Personaje* personaje, const Caparazon& caparazon)
{
    if (!personaje) return;

    float factor = caparazon.getFactorVelocidad();
    personaje->setX(personaje->x() + 1.0 * factor);  // Ejemplo básico
}

// ============================================
// FÍSICA DE SALTO - VERSIÓN BÁSICA
// ============================================

void Fisica::actualizarFisicaSalto(Personaje* personaje, const QList<QGraphicsItem*>& colisiones)
{
    if (personaje->parentItem() && personaje->scene() &&
        !personaje->scene()->items().contains(personaje->parentItem())) {
        personaje->setParentItem(nullptr);
    }

    if (!personaje) return;

    bool soporteEncontrado = false;

    for (QGraphicsItem* item : colisiones) {
        if (dynamic_cast<Plataforma*>(item) || dynamic_cast<QGraphicsRectItem*>(item)) {
            soporteEncontrado = true;
            break;
        }
    }

    float velSalto = personaje->getVelocidadSalto();

    if (personaje->estaSaltando()) {
        // Si está en salto activo (subiendo o cayendo)
        personaje->setY(personaje->y() + velSalto);
        personaje->setVelocidadSalto(velSalto + 1.0f);  // gravedad

        // Limitar velocidad de caída
        if (personaje->getVelocidadSalto() > 10.0f)
            personaje->setVelocidadSalto(10.0f);

        // Si hay soporte y está cayendo, aterriza
        if (soporteEncontrado && personaje->getVelocidadSalto() >= 0) {
            personaje->setVelocidadSalto(0);
            personaje->setEnSalto(false);

            for (QGraphicsItem* item : colisiones) {
                if (Plataforma* plataforma = dynamic_cast<Plataforma*>(item)) {
                    QGraphicsItem* plataformaActual = personaje->parentItem();
                    QGraphicsItem* nuevaPlataforma = plataforma;

                    if (plataformaActual != nuevaPlataforma) {
                        // Cambia de plataforma solo si es diferente
                        QPointF posicionGlobal = personaje->scenePos();
                        personaje->setParentItem(nuevaPlataforma);
                        personaje->setPos(nuevaPlataforma->mapFromScene(posicionGlobal));
                    }

                    QRectF platRect = plataforma->boundingRect();
                    personaje->setY(platRect.top() - personaje->boundingRect().height());
                    break;
                } else if (QGraphicsRectItem* suelo = dynamic_cast<QGraphicsRectItem*>(item)) {
                    personaje->setParentItem(nullptr);
                    QRectF rect = suelo->boundingRect().translated(suelo->scenePos());
                    personaje->setY(rect.top() - personaje->boundingRect().height());
                    break;
                }
            }

            qDebug() << "[Fisica] Aterrizó. EnSalto desactivado.";
        }
    }
    else if (!soporteEncontrado) {
        // Si no está en salto pero no hay soporte (se cayó)
        personaje->setY(personaje->y() + velSalto);
        personaje->setVelocidadSalto(velSalto + 1.0f);

        if (personaje->getVelocidadSalto() > 10.0f)
            personaje->setVelocidadSalto(10.0f);
    }

    // qDebug() << "[Fisica] Y:" << personaje->y()
    //          << " VelSalto:" << personaje->getVelocidadSalto()
    //          << " EnSalto:" << personaje->estaSaltando()
    //          << " Soporte:" << soporteEncontrado;

}

// ============================================
// FÍSICA DE SALTO - VERSIÓN CON AGUA
// ============================================

void Fisica::actualizarFisicaSalto(Personaje* personaje, const QList<QGraphicsItem*>& colisiones, QGraphicsRectItem* zonaAgua)
{
    if (!personaje) return;

    if (personaje->parentItem() && personaje->scene() &&
        !personaje->scene()->items().contains(personaje->parentItem())) {
        personaje->setParentItem(nullptr);
    }

    bool soporteEncontrado = false;

    for (QGraphicsItem* item : colisiones) {
        if (item == zonaAgua) continue;  // No usar agua como soporte

        if (dynamic_cast<Plataforma*>(item) || dynamic_cast<QGraphicsRectItem*>(item)) {
            soporteEncontrado = true;
            break;
        }
    }

    float velSalto = personaje->getVelocidadSalto();

    Goku* goku = dynamic_cast<Goku*>(personaje);
    bool nadando = (goku && goku->estaNadando());

    if (nadando) {
        // Si está nadando, no aplicar gravedad normal.
        velSalto = 0;
    }

    if (personaje->estaSaltando()) {
        personaje->setY(personaje->y() + velSalto);

        if (!nadando) {
            personaje->setVelocidadSalto(velSalto + 1.0f);  // Gravedad normal
            if (personaje->getVelocidadSalto() > 10.0f)
                personaje->setVelocidadSalto(10.0f);
        }

        if (soporteEncontrado && personaje->getVelocidadSalto() >= 0) {
            personaje->setVelocidadSalto(0);
            personaje->setEnSalto(false);

            for (QGraphicsItem* item : colisiones) {
                if (item == zonaAgua) continue;

                if (Plataforma* plataforma = dynamic_cast<Plataforma*>(item)) {
                    QGraphicsItem* plataformaActual = personaje->parentItem();
                    if (plataformaActual != plataforma) {
                        QPointF posicionGlobal = personaje->scenePos();
                        personaje->setParentItem(plataforma);
                        personaje->setPos(plataforma->mapFromScene(posicionGlobal));
                    }
                    QRectF platRect = plataforma->boundingRect();
                    personaje->setY(platRect.top() - personaje->boundingRect().height());
                    break;
                }
                else if (QGraphicsRectItem* suelo = dynamic_cast<QGraphicsRectItem*>(item)) {
                    personaje->setParentItem(nullptr);
                    QRectF rect = suelo->boundingRect().translated(suelo->scenePos());
                    personaje->setY(rect.top() - personaje->boundingRect().height());
                    break;
                }
            }
            qDebug() << "[Fisica] Aterrizó. EnSalto desactivado.";
        }
    }
    else if (!soporteEncontrado && !nadando) {
        // Solo aplica gravedad si NO está nadando
        personaje->setY(personaje->y() + velSalto);
        personaje->setVelocidadSalto(velSalto + 1.0f);
        if (personaje->getVelocidadSalto() > 10.0f)
            personaje->setVelocidadSalto(10.0f);
    }
}

// ============================================
// FÍSICA DEL AGUA - NATACIÓN Y FLOTABILIDAD
// ============================================

void Fisica::aplicarFisicaAgua(Personaje* personaje, QGraphicsRectItem* zonaAgua)
{
    if (!personaje || !zonaAgua) return;

    Goku* goku = dynamic_cast<Goku*>(personaje);
    if (!goku) return;

    QRectF cuerpoGoku = goku->sceneBoundingRect();
    QRectF areaAgua = zonaAgua->sceneBoundingRect();
    bool dentro = areaAgua.intersects(cuerpoGoku);

    if (dentro) {
        if (!goku->estaNadando()) {
            goku->activarNado();
        }

        // Flotabilidad pasiva (si no presiona nada)
        float flotabilidad = goku->getCaparazon().getFactorFlotacion();
        goku->setY(goku->y() - 1.5f * flotabilidad);

    } else {
        if (goku->estaNadando()) {
            goku->desactivarNado();
        }
    }
}

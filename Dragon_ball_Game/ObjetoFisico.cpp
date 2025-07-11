#include "ObjetoFisico.h"
#include "Item.h"
#include "Nivel2.h"
#include "Goku.h"
#include "Obstaculo.h"
#include "Juego.h"
#include <QGraphicsScene>
#include <QBrush>
#include <QPainter>
#include <QDebug>

// ============================================
// CONSTRUCTOR
// ============================================
ObjetoFisico::ObjetoFisico(bool esProyectil, QObject* parent)
    : QObject(parent), velocidadX(0), velocidadY(0), usarGravedad(true), gravedad(0.5f)
{
    if (esProyectil) {
        configurarComoProyectil();
    }
}

// ============================================
// CONFIGURACIÓN DE PROYECTIL
// ============================================
void ObjetoFisico::configurarComoProyectil()
{
    QPixmap pixmap(20, 20);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::white);
    painter.setPen(Qt::black);
    painter.drawEllipse(0, 0, 20, 20);
    setPixmap(pixmap);
    setZValue(3);
    setData(0, "proyectil");
}

// ============================================
// SISTEMA DE COLISIONES
// ============================================
void ObjetoFisico::verificarColisiones()
{
    if (!scene() || destruido) return;
    QList<QGraphicsItem*> colisiones = collidingItems();
    qDebug() << "[Colisiones] Proyectil ha chocado con" << colisiones.size() << "elementos.";

    for (QGraphicsItem* otro : colisiones) {
        // ----- ITEM -----
        if (Item* item = dynamic_cast<Item*>(otro)) {
            if (!item->isRecolectado()) {
                if (Nivel2* nivel = dynamic_cast<Nivel2*>(scene())) {
                    if (Goku* goku = dynamic_cast<Goku*>(nivel->getPersonaje())) {
                        goku->recolectar(item);
                        item->setRecolectado(true);
                        if (Juego* juego = dynamic_cast<Juego*>(nivel->parent())) {
                            juego->actualizarPanelInfo();
                        }
                        nivel->verificarObjetivoCompleto();
                    }
                }
                destruir();
                return;
            }
        }
        // ----- OBJETIVO -----
        if (Obstaculo* obj = dynamic_cast<Obstaculo*>(otro)) {
            if (obj->getTipo() == TipoObstaculo::Objetivo) {
                if (obj->estado() != EstadoObjetivo::Activo) {
                    qDebug() << "[Proyectil] Ignora objetivo NO activo.";
                    continue;
                }
                if (obj->estado() == EstadoObjetivo::Entregado) {
                    qDebug() << "[Proyectil] Ignora objetivo YA entregado.";
                    continue;
                }
                qDebug() << "[Proyectil] Colisión con objetivo ACTIVO:" << static_cast<int>(obj->getTipo());
                obj->setEstado(EstadoObjetivo::Entregado);
                if (Nivel2* nivel = dynamic_cast<Nivel2*>(scene())) {
                    nivel->incrementarObjetivosEntregados();
                    if (Juego* juego = dynamic_cast<Juego*>(nivel->parent())) {
                        juego->actualizarPanelInfo();
                    }
                }
                destruir();
                return;
            }
            // Otros obstaculos = destruye igual
            destruir();
            return;
        }
    }

    if (scene() && !scene()->sceneRect().contains(pos())) {
        destruir();
    }
}

// ============================================
// SETTERS
// ============================================
void ObjetoFisico::setVelocidad(float vx, float vy) {
    velocidadX = vx;
    velocidadY = vy;
}

void ObjetoFisico::setTimer(QTimer* timer) {
    timerMovimiento = timer;
}

// ============================================
// SISTEMA DE DESTRUCCIÓN
// ============================================
void ObjetoFisico::destruir() {
    if (destruido) return;
    destruido = true;
    if (scene()) scene()->removeItem(this);
    if (timerMovimiento) {
        timerMovimiento->stop();
        timerMovimiento->deleteLater();
        timerMovimiento = nullptr;
    }
    deleteLater();
}

// ============================================
// SISTEMA DE FÍSICA
// ============================================
void ObjetoFisico::actualizarFisica() {
    if (destruido) return;
    if (usarGravedad) {
        velocidadY += gravedad;
    }
    moveBy(velocidadX, velocidadY);
    verificarColisiones();
}

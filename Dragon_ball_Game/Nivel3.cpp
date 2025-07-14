#include "Nivel3.h"
#include "Goku.h"
#include "Plataforma.h"
#include "Burbuja.h"
#include "EsferaDragon.h"
#include "Obstaculo.h"
#include "Juego.h"

#include <QGraphicsRectItem>
#include <QTimer>
#include <QDebug>
#include <QPixmap>
#include <QRandomGenerator>
#include <QPropertyAnimation>

// ============================================
// CONSTRUCTOR
// ============================================

Nivel3::Nivel3(QObject *parent)
    : Nivel(parent)
{
    tipo = TipoNivel::TRES;
    limiteInferiorY = 600;

    cargarFondo();
    cargarElementos();

    timerFisica = new QTimer(this);
    connect(timerFisica, &QTimer::timeout, this, &Nivel3::actualizarFisica);
    timerFisica->start(30);
}

// ============================================
// INICIALIZACIÓN
// ============================================

void Nivel3::cargarFondo()
{
    QPixmap fondoPixmap(":/Recursos/backgraunds/BACKGRAUND NIVEL 3.png");
    if (!fondoPixmap.isNull()) {
        fondoPixmap = fondoPixmap.scaled(920, 570, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        fondo = new QGraphicsPixmapItem(fondoPixmap);
        addItem(fondo);
    } else {
        qDebug() << "[Nivel3] No se pudo cargar la imagen de fondo";
    }
}

void Nivel3::cargarElementos()
{
    crearPersonaje();
    agregarItems();
    crearZonaAgua();
    agregarObstaculos();
    agregarPlataformaFinal();
}

void Nivel3::crearPersonaje()
{
    goku = new Goku();
    goku->setPos(100, 500);
    goku->setZValue(2);
    addItem(goku);
}

void Nivel3::crearZonaAgua()
{
    QRectF zonaAguaRect(0, 240, 920, 570);
    zonaAgua = new QGraphicsRectItem(zonaAguaRect);
    zonaAgua->setPen(Qt::NoPen);
    zonaAgua->setBrush(Qt::NoBrush);
    zonaAgua->setZValue(0);
    addItem(zonaAgua);
}

void Nivel3::agregarItems()
{
    for (int i = 0; i < 3; ++i) {
        int xBase = 200 + i * 200;
        QRectF zonaSuperior(xBase, 300, 120, 100);
        QRectF zonaInferior(xBase, 440, 120, 100);

        bool esferaEnZonaSuperior = QRandomGenerator::global()->bounded(2);
        QRectF zonaEsfera = esferaEnZonaSuperior ? zonaSuperior : zonaInferior;
        QPointF posEsfera = puntoAleatorioEnRect(zonaEsfera);
        EsferaDragon* esfera = new EsferaDragon();
        esfera->setPos(posEsfera);
        addItem(esfera);
        esferas.append(esfera);

        QRectF zonaBurbuja = esferaEnZonaSuperior ? zonaInferior : zonaSuperior;
        QPointF posBurbuja = puntoAleatorioEnRect(zonaBurbuja);
        Burbuja* burbuja = new Burbuja();
        burbuja->setPos(posBurbuja);
        addItem(burbuja);
        burbujas.append(burbuja);
    }
}

QPointF Nivel3::puntoAleatorioEnRect(const QRectF& rect)
{
    qreal x = rect.left() + QRandomGenerator::global()->generateDouble() * rect.width();
    qreal y = rect.top() + QRandomGenerator::global()->generateDouble() * rect.height();
    return QPointF(x, y);
}

void Nivel3::agregarObstaculos()
{
    timerObstaculos = new QTimer(this);
    connect(timerObstaculos, &QTimer::timeout, this, [=]() {
        Obstaculo* escombro = new Obstaculo(TipoObstaculo::Escombro, 0);
        escombro->setPos(QRandomGenerator::global()->bounded(0, 790), 0);
        addItem(escombro);
        escombros.append(escombro);

        Obstaculo* pez = new Obstaculo(TipoObstaculo::Pez, 0);
        pez->setPos(920, QRandomGenerator::global()->bounded(240, 570));
        addItem(pez);
        peces.append(pez);
    });
    timerObstaculos->start(3000);
}

void Nivel3::agregarPlataformaFinal()
{
    plataformaSalida = new Plataforma(TipoPlataforma::FIJA, 800, 500, 120);
    plataformaSalida->setZValue(1);
    addItem(plataformaSalida);
}

// ============================================
// FÍSICA
// ============================================

void Nivel3::actualizarFisica()
{
    if (!goku) return;

    aplicarFisicaBasica();
    manejarColisiones();
    verificarCaidaFueraDelLimite();
}

void Nivel3::aplicarFisicaBasica()
{
    fisica.aplicarFisicaAgua(goku, zonaAgua);
    for (Obstaculo* obs : escombros) fisica.afectarObjeto(obs);
    for (Obstaculo* pez : peces) pez->actualizarMovimiento(goku);
}

// ============================================
// COLISIONES
// ============================================

void Nivel3::manejarColisiones()
{
    QList<QGraphicsItem*> colisiones = goku->collidingItems();

    for (QGraphicsItem* item : colisiones) {
        if (auto* burbuja = dynamic_cast<Burbuja*>(item)) {
            goku->recolectar(burbuja);
            burbujas.removeOne(burbuja);
            burbuja->deleteLater();

            if (goku->getBurbujasRecolectadas() >= 3) {
                goku->setVidas(goku->getVidas() + 1);
                qDebug() << "[Nivel3] Goku ganó una vida extra.";
            }
        } else if (auto* esfera = dynamic_cast<EsferaDragon*>(item)) {
            goku->recolectar(esfera);
            esferas.removeOne(esfera);
            esfera->deleteLater();
        } else if (auto* obs = dynamic_cast<Obstaculo*>(item)) {
            manejarColisionConObstaculo(obs);
        } else if (auto* plataforma = dynamic_cast<Plataforma*>(item)) {
            if (plataforma == plataformaSalida) procesarVictoriaSiCorresponde();
        }
    }
}

void Nivel3::manejarColisionConObstaculo(Obstaculo* obs)
{
    if (!obs || goku->getInvulnerable()) return;

    if (obs->getTipo() == TipoObstaculo::Escombro || obs->getTipo() == TipoObstaculo::Pez) {
        goku->setVidas(goku->getVidas() - 1);
        goku->activarInvulnerabilidad(2000);
        qDebug() << "[Nivel3] Goku golpeado por" << (obs->getTipo() == TipoObstaculo::Escombro ? "Escombro" : "Pez")
                 << ". Vidas:" << goku->getVidas();

        // Solo quitar si aún pertenece a escena
        if (obs->scene()) removeItem(obs);
        escombros.removeOne(obs);
        peces.removeOne(obs);
        obs->deleteLater();

        if (goku->getVidas() > 0) {
            reiniciarNivel();
        } else {
            manejarGameOver();
        }
    }
}

// ============================================
// LÍMITE INFERIOR
// ============================================

void Nivel3::verificarCaidaFueraDelLimite()
{
    if (goku->y() > limiteInferiorY) {
        goku->setVidas(goku->getVidas() - 1);
        goku->activarInvulnerabilidad(2000);

        if (goku->getVidas() <= 0) {
            manejarGameOver();
        } else {
            reiniciarNivel();
        }
    }
}

// ============================================
// VICTORIA
// ============================================

void Nivel3::procesarVictoriaSiCorresponde()
{
    if (!verificarObjetivoCompleto() || emitidoObjetivoCumplido) return;

    emitidoObjetivoCumplido = true;

    emit pausarTiempo();
    goku->activarInvulnerabilidad(999999); // Invulnerable hasta terminar animación
    timerFisica->stop();
    timerObstaculos->stop();

    for (Obstaculo* pez : peces) pez->setVelocidad(0, 0);
    for (Obstaculo* esc : escombros) esc->setVelocidad(0, 0);

    QTimer* timerSalida = new QTimer(this);
    connect(timerSalida, &QTimer::timeout, [=]() mutable {
        float nuevaY = plataformaSalida->y() - 2.0f;
        plataformaSalida->setY(nuevaY);
        goku->setY(goku->y() - 2.0f);

        if (nuevaY <= 300) {
            timerSalida->stop();
            timerSalida->deleteLater();
            emit objetivoCumplido();
        }
    });
    timerSalida->start(40);
}

bool Nivel3::verificarObjetivoCompleto()
{
    if (!goku) return false;
    Juego* juego = dynamic_cast<Juego*>(parent());
    bool esModoHistoria = (juego && juego->getModoHistoria());
    return goku->getEsferasRecolectadas() >= (esModoHistoria ? 7 : 3);
}

// ============================================
// REINICIO Y GAME OVER
// ============================================

void Nivel3::reiniciarNivel()
{
    if (!goku) return;

    goku->setPos(100, 500);
    goku->setVelocidad(0, 0);
    goku->setVelocidadSalto(0);
    goku->setEnSalto(false);
    goku->activarInvulnerabilidad(2000);

    emit reinicioCompletado();
}

void Nivel3::manejarGameOver()
{
    qDebug() << "[Nivel3] GAME OVER";

    for (Obstaculo* pez : peces) {
        if (pez && pez->scene()) removeItem(pez);
        pez->deleteLater();
    }
    peces.clear();

    for (Obstaculo* esc : escombros) {
        if (esc && esc->scene()) removeItem(esc);
        esc->deleteLater();
    }
    escombros.clear();

    emit vidasAgotadas();
}

Personaje* Nivel3::getPersonaje() const
{
    return goku;
}

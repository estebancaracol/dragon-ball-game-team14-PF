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
// INICIALIZACIÓN DEL NIVEL
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

// ============================================
// CREACIÓN DE PERSONAJE
// ============================================
void Nivel3::crearPersonaje()
{
    goku = new Goku();
    goku->setPos(100, 500);
    goku->setZValue(2); // sobre la plataforma
    addItem(goku);
}

// ============================================
// CREACIÓN DE ZONA DE AGUA
// ============================================
void Nivel3::crearZonaAgua()
{
    QRectF zonaAguaRect(0, 240, 920, 570);
    zonaAgua = new QGraphicsRectItem(zonaAguaRect);
    zonaAgua->setPen(Qt::NoPen);
    zonaAgua->setBrush(Qt::NoBrush);
    zonaAgua->setZValue(0);
    addItem(zonaAgua);
}

// ============================================
// CREACIÓN DE ITEMS COLECCIONABLES
// ============================================
void Nivel3::agregarItems() {
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

QPointF Nivel3::puntoAleatorioEnRect(const QRectF& rect) {
    qreal x = rect.left() + QRandomGenerator::global()->generateDouble() * rect.width();
    qreal y = rect.top() + QRandomGenerator::global()->generateDouble() * rect.height();
    return QPointF(x, y);
}

// ============================================
// CREACIÓN DE OBSTÁCULOS
// ============================================
void Nivel3::agregarObstaculos()
{
    timerObstaculos = new QTimer(this);
    connect(timerObstaculos, &QTimer::timeout, this, [=]() {
        Obstaculo* escombro = new Obstaculo(TipoObstaculo::Escombro, 0);
        int xCielo = QRandomGenerator::global()->bounded(0, 790);
        escombro->setPos(xCielo, 0);
        addItem(escombro);
        escombros.append(escombro);

        Obstaculo* pez = new Obstaculo(TipoObstaculo::Pez, 0);
        int yAgua = QRandomGenerator::global()->bounded(240, 570);
        pez->setPos(920, yAgua);
        addItem(pez);
        peces.append(pez);
        qDebug() << "[Nivel3] Pez creado en Y:" << yAgua;
    });
    timerObstaculos->start(3000);
}

// ============================================
// CREACIÓN DE PLATAFORMA FINAL
// ============================================
void Nivel3::agregarPlataformaFinal()
{
    plataformaSalida = new Plataforma(TipoPlataforma::FIJA, 800, 500, 120);
    plataformaSalida->setZValue(1);
    addItem(plataformaSalida);
}

// ============================================
// SISTEMA DE FÍSICA
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
// SISTEMA DE COLISIONES
// ============================================
void Nivel3::manejarColisiones()
{
    QList<QGraphicsItem*> colisiones = goku->collidingItems();

    for (QGraphicsItem* item : colisiones) {
        if (auto* burbuja = dynamic_cast<Burbuja*>(item)) {
            goku->recolectar(burbuja);
            burbujas.removeOne(burbuja);
            delete burbuja;

            if (goku->getBurbujasRecolectadas() >= 3) {
                goku->setVidas(goku->getVidas() + 1);
                qDebug() << "[Nivel3] Goku ganó una vida extra por recolectar 3 burbujas.";
            }
        }
        else if (auto* esfera = dynamic_cast<EsferaDragon*>(item)) {
            goku->recolectar(esfera);
            esferas.removeOne(esfera);
            delete esfera;
        }
        else if (auto* obs = dynamic_cast<Obstaculo*>(item)) {
            manejarColisionConObstaculo(obs);
        }
        else if (auto* plataforma = dynamic_cast<Plataforma*>(item)) {
            if (plataforma == plataformaSalida) procesarVictoriaSiCorresponde();
        }
    }
}

void Nivel3::manejarColisionConObstaculo(Obstaculo* obs)
{
    if (!obs || goku->getInvulnerable()) return;

    if (obs->getTipo() == TipoObstaculo::Escombro) {
        goku->setVidas(goku->getVidas() - 1);
        goku->activarInvulnerabilidad();
        qDebug() << "[Nivel3] Goku golpeado por escombro. Vidas:" << goku->getVidas();

        if (goku->getVidas() > 0) {
            emit vidasAgotadas();
            reiniciarNivel();
        } else {
            emit vidasAgotadas();
        }

        removeItem(obs);
        escombros.removeOne(obs);
        delete obs;
    }
    else if (obs->getTipo() == TipoObstaculo::Pez) {
        obs->registrarAtaque(goku);
        if (obs->getAtaquesRestantes() <= 0) {
            goku->activarInvulnerabilidad();
            if (goku->getVidas() > 0) {
                emit vidasAgotadas();
                reiniciarNivel();
            } else {
                emit vidasAgotadas();
            }
            removeItem(obs);
            peces.removeOne(obs);
            delete obs;
        }
    }
}

// ============================================
// VERIFICACIÓN DE LÍMITES
// ============================================
void Nivel3::verificarCaidaFueraDelLimite()
{
    if (goku->y() > limiteInferiorY) {
        goku->setVidas(goku->getVidas() - 1);
        if (goku->getVidas() > 0) {
            emit vidasAgotadas();
            reiniciarNivel();
        } else {
            emit vidasAgotadas();
        }
    }
}

// ============================================
// SISTEMA DE VICTORIA
// ============================================
void Nivel3::procesarVictoriaSiCorresponde()
{
    if (!verificarObjetivoCompleto() || emitidoObjetivoCumplido) return;
    emitidoObjetivoCumplido = true;
    qDebug() << "[Nivel3] Requisitos completos. Activando animación de salida.";

    emit pausarTiempo();
    goku->activarInvulnerabilidad();
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
            qDebug() << "[Nivel3] Plataforma de salida completó ascenso. Emitiendo objetivoCumplido()";
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

    int esferasRecolectadas = goku->getEsferasRecolectadas();
    int esferasRequeridas = esModoHistoria ? 7 : 3;

    return esferasRecolectadas >= esferasRequeridas;
}

// ============================================
// SISTEMA DE REINICIO
// ============================================
void Nivel3::reiniciarNivel()
{
    qDebug() << "[Nivel3] Reiniciando nivel...";

    goku->setParentItem(nullptr);
    goku->setPos(100, 500);
    goku->setVelocidad(0, 0);
    goku->setVelocidadSalto(0);
    goku->setEnSalto(false);
    goku->activarInvulnerabilidad();

    emit reinicioCompletado();
}

// ============================================
// GETTERS
// ============================================
Personaje* Nivel3::getPersonaje() const
{
    return goku;
}

#include "Juego.h"
#include "Nivel2.h"
#include "Goku.h"
#include "Plataforma.h"
#include "CajaLeche.h"
#include "EsferaDragon.h"
#include "Obstaculo.h"

#include <QGraphicsRectItem>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QTimer>
#include <stdexcept>

// ============================================
// CONSTRUCTOR
// ============================================

Nivel2::Nivel2(QObject *parent)
    : Nivel(parent), goku(nullptr)
{
    tipo = TipoNivel::DOS;
    cargarFondo();
    cargarElementos();

    timerFisica = new QTimer(this);
    connect(timerFisica, &QTimer::timeout, this, &Nivel2::actualizarFisica);
    timerFisica->start(30);
}

// ============================================
// SISTEMA DE CARGA DE NIVEL
// ============================================

void Nivel2::cargarFondo()
{
    QPixmap fondoPixmap(":/Recursos/backgraunds/BACKGRAUND NIVEL 2.png");

    if (!fondoPixmap.isNull()) {
        fondoPixmap = fondoPixmap.scaled(920, 570, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        fondo = new QGraphicsPixmapItem(fondoPixmap);
        addItem(fondo);
    } else {
        qDebug() << "[Nivel2] No se pudo cargar la imagen de fondo";
    }
}

void Nivel2::cargarElementos()
{
    crearPersonaje();
    crearSuelosInvisibles();
    agregarPlataformaAscensor();
    agregarPlataformasMoviles();
    agregarObjetivoEnPlataformas();
    iniciarCambioObjetivo();
    agregarItems();
}

void Nivel2::crearPersonaje()
{
    goku = new Goku();
    goku->setPos(50, 50);
    goku->setZValue(1);
    addItem(goku);
}

void Nivel2::crearSuelosInvisibles()
{
    QList<QRectF> zonasSuelo = {
        QRectF(40, 520, 270, 50),
        QRectF(650, 520, 250, 50),
    };

    for (const QRectF &zona : zonasSuelo) {
        QGraphicsRectItem* suelo = new QGraphicsRectItem(zona);
        suelo->setZValue(0);
        suelo->setPen(Qt::NoPen);
        suelo->setBrush(Qt::NoBrush);
        addItem(suelo);
        suelosInvisibles.append(suelo);
    }
}

// ============================================
// SISTEMA DE PLATAFORMAS
// ============================================

void Nivel2::agregarPlataformaAscensor()
{
    float x = 170;
    float y = 400;
    float ancho = 120;

    Plataforma* ascensor = new Plataforma(TipoPlataforma::MOVIL, x, y, ancho);

    ascensor->setProperty("modoAscensorVertical", true);  // Nuevo modo
    ascensor->setProperty("ySuperior", 100.0f);           // Movimiento amplio
    ascensor->setProperty("yInferior", 500.0f);
    ascensor->setProperty("direccionVertical", -1);       // Inicia subiendo
    ascensor->setProperty("timerSacudida", 0);            // Contador sacudida

    plataformas.append(ascensor);
    addItem(ascensor);

    // Espera de 5 segundos antes de comenzar el movimiento
    QTimer::singleShot(5000, ascensor, [ascensor]() {
        QObject::connect(ascensor->getTimerMovimiento(), &QTimer::timeout, ascensor, &Plataforma::moverAscensorVertical);
        ascensor->getTimerMovimiento()->start(50);
    });
}

void Nivel2::agregarPlataformasMoviles() {
    QList<QPointF> posiciones = {
        QPointF(700, 80),
        QPointF(700, 180),
        QPointF(700, 280),
        QPointF(700, 380),
        QPointF(700, 480)
    };

    for (const QPointF& pos : posiciones) {
        Plataforma* p = new Plataforma(TipoPlataforma::FIJA, pos.x(), pos.y(), 120);
        p->setPos(pos);
        addItem(p);
        plataformas.append(p);
        plataformasObjetivo.append(p);
    }
}

// ============================================
// SISTEMA DE OBJETIVOS
// ============================================

void Nivel2::agregarObjetivoEnPlataformas() {
    if (plataformasObjetivo.size() < 5) return;

    for (int i = 0; i < plataformasObjetivo.size(); ++i) {
        Plataforma* plataforma = plataformasObjetivo[i];
        Obstaculo* objetivo = new Obstaculo(TipoObstaculo::Objetivo, 0);
        objetivo->setZValue(2);
        objetivo->setVisible(true);

        objetivo->setPos(plataforma->x() + 45, plataforma->y() - 75);

        addItem(objetivo);
        objetivos.append(objetivo);
    }
}

void Nivel2::iniciarCambioObjetivo()
{
    if (objetivos.isEmpty()) return;

    // Timer para cambiar de objetivo cada 8 segundos (5 activos + 3 parpadeo)
    QTimer* timerCambio = new QTimer(this);

    connect(timerCambio, &QTimer::timeout, this, [=]() {
        // 1) Desactiva todos los objetivos
        for (Obstaculo* obj : objetivos) {
            if (obj->estado() != EstadoObjetivo::Entregado) {
                obj->setEstado(EstadoObjetivo::Inactivo);
            }
        }

        // 2) Escoge uno aleatorio no entregado
        QList<Obstaculo*> disponibles;
        for (Obstaculo* obj : objetivos) {
            if (obj->estado() != EstadoObjetivo::Entregado) {
                disponibles.append(obj);
            }
        }

        if (disponibles.isEmpty()) {
            timerCambio->stop();
            timerCambio->deleteLater();
            return; // No hay más objetivos
        }

        int indice = QRandomGenerator::global()->bounded(disponibles.size());
        Obstaculo* elegido = disponibles[indice];

        // 3) Actívalo
        elegido->setEstado(EstadoObjetivo::Activo);

        // 4) Después de 5 seg, inicia parpadeo
        QTimer::singleShot(5000, this, [=]() {
            // Evita volver a parpadear si ya fue entregado en esos 5 seg
            if (elegido->estado() == EstadoObjetivo::Activo) {
                elegido->setEstado(EstadoObjetivo::Parpadeando);
            }
        });
    });

    timerCambio->start(8000); // ciclo de 8 seg (5 + 3 parpadeo)
}

void Nivel2::incrementarObjetivosEntregados() {
    objetivosEntregados++;
    emit objetivoEntregado(objetivosEntregados);

    qDebug() << "[Nivel2] Objetivos entregados:" << objetivosEntregados;

    // Si ya cumplió todo, lanza la señal como Nivel1
    if (verificarObjetivoCompleto()) {
        qDebug() << "[Nivel2] ¡Requisitos completos! Emitiendo objetivoCumplido()";
        emit objetivoCumplido();
    }
}

bool Nivel2::verificarObjetivoCompleto() {
    if (!goku) {
        qDebug() << "[Nivel2] Goku no está inicializado.";
        return false;
    }

    // Obtener el puntero al juego para verificar si está en modo historia
    Juego* juego = dynamic_cast<Juego*>(parent());
    bool esModoHistoria = (juego && juego->getModoHistoria());

    int esferasRecolectadas = goku->getEsferasRecolectadas();
    int esferasRequeridas = esModoHistoria ? 4 : 2;

    bool cumpleEsferas = esferasRecolectadas >= esferasRequeridas;
    bool cumpleObjetivos = objetivosEntregados >= 5;

    bool resultado = (cumpleEsferas && cumpleObjetivos);

    return resultado;
}

// ============================================
// SISTEMA DE ITEMS
// ============================================

void Nivel2::agregarItems() {
    QRectF zonaSuperior(360, 80, 200, 150);
    QRectF zonaInferior(360, 330, 200, 150);

    // Esferas (igual que antes)
    bool primeroArriba = QRandomGenerator::global()->bounded(2);
    QRectF zonaPrimera = primeroArriba ? zonaSuperior : zonaInferior;
    QRectF zonaSegunda = primeroArriba ? zonaInferior : zonaSuperior;


    QTimer::singleShot(30 * 1000, this, [=]() {
        EsferaDragon* esfera1 = new EsferaDragon();
        float x1 = zonaPrimera.x() + QRandomGenerator::global()->bounded(int(zonaPrimera.width()) - 40);
        float y1 = zonaPrimera.y() + QRandomGenerator::global()->bounded(int(zonaPrimera.height()) - 40);
        esfera1->setPos(x1, y1);
        esfera1->setData(0, "esfera");
        addItem(esfera1);
        qDebug() << "[Nivel2] Esfera 1 agregada en:" << x1 << y1;

        EsferaDragon* esfera2 = new EsferaDragon();
        float x2 = zonaSegunda.x() + QRandomGenerator::global()->bounded(int(zonaSegunda.width()) - 40);
        float y2 = zonaSegunda.y() + QRandomGenerator::global()->bounded(int(zonaSegunda.height()) - 40);
        esfera2->setPos(x2, y2);
        esfera2->setData(0, "esfera");
        addItem(esfera2);
        qDebug() << "[Nivel2] Esfera 2 agregada en:" << x2 << y2;
    });

    // Crear cajas cada 30 segundos
    QTimer* timerCajas = new QTimer(this);
    connect(timerCajas, &QTimer::timeout, this, [=]() {
        for (int i = 0; i < 2; ++i) {
            QRectF zona = QRandomGenerator::global()->bounded(2) ? zonaSuperior : zonaInferior;
            int tiros = QRandomGenerator::global()->bounded(2, 6);  // 2 a 5

            float x = zona.x() + QRandomGenerator::global()->bounded(int(zona.width()) - 40);
            float y = zona.y() + QRandomGenerator::global()->bounded(int(zona.height()) - 40);

            CajaLeche* caja = new CajaLeche(tiros);
            caja->setPos(x, y);
            addItem(caja);

            qDebug() << "[Nivel2] Caja de leche creada con" << tiros << "tiros en:" << x << y;

            caja->iniciarDesaparicionConParpadeo(10000, 1500);
        }
    });

    timerCajas->start(30 * 1000);
}

// ============================================
// SISTEMA DE FÍSICAS Y COLISIONES
// ============================================

void Nivel2::actualizarFisica()
{
    if (!goku) return;

    QList<QGraphicsItem*> colisiones = goku->collidingItems();
    fisica.actualizarFisicaSalto(goku, colisiones);

    for (QGraphicsItem* item : colisiones) {
        if (!item || !this->items().contains(item)) continue;

        // Plataforma
        if (Plataforma* plataforma = dynamic_cast<Plataforma*>(item)) {
            plataforma->detectarColisionConGoku(goku);
        }

        // Ítems recolectables
        if (Item* itemRecolectable = dynamic_cast<Item*>(item)) {
            if (!itemRecolectable->isRecolectado()) {
                goku->recolectar(itemRecolectable);
            }
        }

        // Obstáculo - Objetivo
        if (Obstaculo* obj = dynamic_cast<Obstaculo*>(item)) {
            if (obj->getTipo() == TipoObstaculo::Objetivo) {
                if (obj->estado() == EstadoObjetivo::Activo) {
                    ++objetivosEntregados;
                    obj->setEstado(EstadoObjetivo::Entregado);
                    emit objetivoEntregado(objetivosEntregados);
                    qDebug() << "[Nivel2] Objetivo entregado. Total:" << objetivosEntregados;
                }
            }
        }
    }

    if (verificarObjetivoCompleto()) {
        if (!emitidoObjetivoCumplido) {
            emitidoObjetivoCumplido = true;
            qDebug() << "[Nivel2] Emitiendo señal objetivoCumplido() desde actualizarFisica()";
            emit objetivoCumplido();
        }
    }

    if (goku->y() > limiteInferiorY) {
        goku->setVidas(goku->getVidas() - 1);

        if (goku->getVidas() <= 0) {
            emit vidasAgotadas();
        } else {
            reiniciarNivel();
        }
    }
}

// ============================================
// SISTEMA DE REINICIO DE NIVEL
// ============================================

void Nivel2::reiniciarNivel()
{
    qDebug() << "[Nivel2] Reiniciando nivel...";

    goku->setParentItem(nullptr);
    goku->setPos(50, 50);
    goku->setVelocidad(0, 0);
    goku->setVelocidadSalto(0);
    goku->setEnSalto(false);

    emitidoObjetivoCumplido = false;

    emit reinicioCompletado();
}

// ============================================
// GETTERS Y UTILIDADES
// ============================================

Personaje* Nivel2::getPersonaje() const {
    return goku;
}

int Nivel2::getCantidadObjetivosEntregados() const {
    return objetivosEntregados; // variable que vas actualizando con cada colisión
}

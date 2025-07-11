#include "Juego.h"
#include "Nivel1.h"
#include "Plataforma.h"
#include "Goku.h"
#include "CajaLeche.h"
#include "EsferaDragon.h"

#include <QPixmap>
#include <QDebug>
#include <QGraphicsRectItem>
#include <QTimer>

// ============================================
// CONSTRUCTOR
// ============================================

Nivel1::Nivel1(QObject *parent)
    : Nivel(parent), goku(nullptr), timerCaida(new QTimer(this)), velocidadCaida(2.0f)
{
    tipo = TipoNivel::UNO;
    cargarFondo();
    cargarElementos();

    timerFisica = new QTimer(this);
    connect(timerFisica, &QTimer::timeout, this, &Nivel1::actualizarFisica);
    timerFisica->start(30);
}

// ============================================
// SISTEMA DE CARGA DE NIVEL
// ============================================

void Nivel1::cargarFondo()
{
    QPixmap fondoPixmap(":/Recursos/backgraunds/BACKGRAUND NIVEL 1.png");
    if (!fondoPixmap.isNull()) {
        fondoPixmap = fondoPixmap.scaled(920, 570, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        fondo = new QGraphicsPixmapItem(fondoPixmap);
        addItem(fondo);
    } else {
        qDebug() << "[Nivel1] No se pudo cargar la imagen de fondo";
    }
}

void Nivel1::cargarElementos()
{
    crearPersonaje();
    crearSuelosInvisibles();
    agregarPlataformas();
    agregarItems();
}

void Nivel1::crearPersonaje()
{
    goku = new Goku();
    goku->setPos(0, 0);
    goku->setZValue(1);
    addItem(goku);
}

void Nivel1::crearSuelosInvisibles()
{
    QList<QRectF> zonasSuelo = {
        QRectF(0, 520, 320, 30),
        QRectF(460, 520, 455, 30)
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

void Nivel1::agregarPlataformas()
{
    QList<QPointF> posiciones = {
        {200, 450}, {350, 350}, {200, 250},
        {350, 150}, {500, 250}, {650, 400},
        {760, 250}, {100, 100}, {650, 100}
    };

    for (int i = 0; i < posiciones.size(); ++i) {
        Plataforma* p = new Plataforma(TipoPlataforma::MOVIL, posiciones[i].x(), posiciones[i].y(), 100.0f);
        if (i == 8) {
            p->setEsFinal(true); // Marcar plataforma final
        }

        p->setProperty("xAnterior", p->x());
        p->setProperty("yAnterior", p->y());

        plataformas.append(p);
        addItem(p);
    }
}

void Nivel1::agregarItems()
{
    if (plataformas.size() < 9) return;

    EsferaDragon* esfera1 = new EsferaDragon(100);
    esfera1->setParentItem(plataformas[2]);
    esfera1->setPos(25, -40);

    CajaLeche* caja1 = new CajaLeche(5);
    caja1->setParentItem(plataformas[4]);
    caja1->setPos(25, -35);

    CajaLeche* caja2 = new CajaLeche(3);
    caja2->setParentItem(plataformas[7]);
    caja2->setPos(25, -35);

    EsferaDragon* esfera2 = new EsferaDragon(200);
    esfera2->setParentItem(plataformas[8]);
    esfera2->setPos(25, -40);
}

// ============================================
// SISTEMA DE FÍSICAS Y COLISIONES
// ============================================

void Nivel1::actualizarFisica()
{
    if (!goku) return;

    QList<QGraphicsItem*> colisiones = goku->collidingItems();
    fisica.actualizarFisicaSalto(goku, colisiones);

    bool recolectoItem = false;

    for (QGraphicsItem* item : colisiones) {
        if (!item || !this->items().contains(item)) continue;

        if (Plataforma* plataforma = dynamic_cast<Plataforma*>(item)) {
            plataforma->detectarColisionConGoku(goku);
        }

        if (Item* itemRecolectable = dynamic_cast<Item*>(item)) {
            if (!itemRecolectable->isRecolectado()) {
                goku->recolectar(itemRecolectable);
                recolectoItem = true;  // ← Marcar que se recolectó algo
            }
        }
    }

    //Verificar si el objetivo se cumple después de recolectar
    if (recolectoItem) {
        verificarObjetivoCompleto();
    }

    if (goku->y() > limiteInferiorY) {
        goku->setVidas(goku->getVidas() - 1);
        emit vidasAgotadas();
        reiniciarNivel();
    }
}

// ============================================
// SISTEMA DE REINICIO DE NIVEL
// ============================================

void Nivel1::reiniciarNivel()
{
    qDebug() << "[Nivel1] Reiniciando nivel...";

    goku->setParentItem(nullptr);
    goku->setPos(0, 0);
    goku->setVelocidad(0, 0);
    goku->setVelocidadSalto(0);
    goku->setEnSalto(false);

    emit reinicioCompletado();
}

// ============================================
// SISTEMA DE OBJETIVOS Y LOGROS
// ============================================

bool Nivel1::verificarObjetivoCompleto()
{
    if (objetivoCompletado) return true;
    if (!goku) return false;

    bool tieneDosEsferas = (goku->getEsferasRecolectadas() >= 2);
    bool tieneCajaObligatoria = goku->tieneCajaLecheObligatoria();

    qDebug() << "[Nivel1] Verificando condiciones:";
    qDebug() << "         - Esferas recolectadas:" << goku->getEsferasRecolectadas();
    qDebug() << "         - Tiene caja obligatoria:" << tieneCajaObligatoria;

    if (tieneDosEsferas && tieneCajaObligatoria) {
        qDebug() << "[Nivel1] ¡Requisitos completos! Activando animación de salida.";

        emit pausarTiempo();

        objetivoCompletado = true;
        detenerTodasLasPlataformas();
        animarPlataformaFinalYMostrarVentana();
        return true;
    }

    return false;
}

bool Nivel1::estaEnPlataformaFinal() const {
    if (!goku || plataformas.size() < 9) return false;
    QGraphicsItem* parentItem = goku->parentItem();
    return (parentItem == plataformas[8]);
}

// ============================================
// SISTEMA DE ANIMACIONES Y FINALIZACION
// ============================================

void Nivel1::detenerTodasLasPlataformas()
{
    if (plataformasDetenidas) return;

    qDebug() << "[Nivel1] Deteniendo todas las plataformas...";

    for (Plataforma* plataforma : plataformas) {
        QTimer* timer = plataforma->findChild<QTimer*>();
        if (timer) timer->stop();
    }

    plataformasDetenidas = true;
}

void Nivel1::animarPlataformaFinalYMostrarVentana()
{
    if (plataformas.size() < 9) return;

    Plataforma* plataformaFinal = plataformas[8];
    QTimer* timerBajada = new QTimer(this);

    connect(timerBajada, &QTimer::timeout, [=]() mutable {
        float nuevaY = plataformaFinal->y() + 2.0f;
        plataformaFinal->setY(nuevaY);

        if (nuevaY >= 520) {
            timerBajada->stop();
            timerBajada->deleteLater();

            qDebug() << "[Nivel1] Plataforma final llegó al suelo. Mostrando ventana de nivel completado.";
            emit objetivoCumplido();
        }
    });

    timerBajada->start(40);
}

// ============================================
// GETTERS Y UTILIDADES
// ============================================

Personaje* Nivel1::getPersonaje() const {
    return goku;
}

#include "Plataforma.h"
#include "Goku.h"

#include <QGraphicsScene>
#include <QPropertyAnimation>
#include <QtMath>

// ============================================
// CONSTRUCTOR Y CONFIGURACIÓN INICIAL
// ============================================

Plataforma::Plataforma(TipoPlataforma tipo, float x, float y, float ancho, float /*tiempo*/)
    : tipo(tipo), ancho(ancho)
{
    setPixmap(QPixmap(":/Recursos/objetos/plataforma.png").scaled(ancho, 20, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setPos(x, y);

    if (tipo == TipoPlataforma::MOVIL) {
        velocidad = 1.5f + QRandomGenerator::global()->generateDouble() * (2.5f - 1.0f);
        direccion = -1.0f + QRandomGenerator::global()->generateDouble() * 2.0f;
        if (qAbs(direccion) < 0.3f)
            direccion = direccion < 0 ? -0.5f : 0.5f;
        moverEnX = QRandomGenerator::global()->bounded(2);

        limiteX1 = x - 50;
        limiteX2 = x + 50;
        limiteY1 = y - 40;
        limiteY2 = y + 40;

        timerMovimiento = new QTimer(this);
        connect(timerMovimiento, &QTimer::timeout, this, &Plataforma::mover);
        timerMovimiento->start(50);

        timerCambioDireccion = new QTimer(this);
        connect(timerCambioDireccion, &QTimer::timeout, this, &Plataforma::cambiarMovimientoAleatorio);
        timerCambioDireccion->start(QRandomGenerator::global()->bounded(1000, 3000));
    }
}

// ============================================
// SISTEMA DE MOVIMIENTO HORIZONTAL/VERTICAL
// ============================================

void Plataforma::mover() {
    if (moverEnX) {
        float nuevaX = x() + direccion * velocidad;
        if (nuevaX < limiteX1 || nuevaX > limiteX2)
            direccion *= -1;
        else
            setX(nuevaX);
    } else {
        float nuevaY = y() + direccion * velocidad;
        if (nuevaY < limiteY1 || nuevaY > limiteY2)
            direccion *= -1;
        else
            setY(nuevaY);
    }
}

void Plataforma::moverAscensorVertical()
{
    float ySup = property("ySuperior").toFloat();
    float yInf = property("yInferior").toFloat();
    int dir = property("direccionVertical").toInt();

    float nuevaY = y() + dir * velocidad;

    if (nuevaY <= ySup || nuevaY >= yInf) {
        dir *= -1; // Cambiar dirección
        setProperty("direccionVertical", dir);
    } else {
        setY(nuevaY);
    }

    // --- SACUDIDA CADA ~30 SEGUNDOS ---
    int contador = property("timerSacudida").toInt();
    contador++;

    if (contador >= 750) {
        contador = 0;

        // Sacudida garantizada cada 30s (sin probabilidad)
        qDebug() << "[Plataforma] Tiempo alcanzado: generando sacudida";
        generarSacudida();
    }

    setProperty("timerSacudida", contador);
}

void Plataforma::cambiarMovimientoAleatorio() {
    direccion = -1.0f + QRandomGenerator::global()->generateDouble() * 2.0f;
    if (std::abs(direccion) < 0.3f)
        direccion = direccion < 0 ? -0.5f : 0.5f;

    moverEnX = QRandomGenerator::global()->bounded(2);
    velocidad = 2.5f + QRandomGenerator::global()->generateDouble() * (5.0f - 2.5f);

    int nuevoIntervalo = QRandomGenerator::global()->bounded(1000, 2000);
    timerCambioDireccion->start(nuevoIntervalo);
}

// ============================================
// EFECTOS ESPECIALES - SACUDIDA
// ============================================

void Plataforma::generarSacudida()
{
    if (!scene()) return;

    QTimer* sacudida = new QTimer(this);
    int* ciclos = new int(0); // contador dinámico

    connect(sacudida, &QTimer::timeout, this, [=]() mutable {
        if (*ciclos >= 10) {
            sacudida->stop();
            sacudida->deleteLater();
            delete ciclos;
            return;
        }

        float offset = (*ciclos % 2 == 0) ? 15.0f : -15.0f;
        qDebug() << "[Plataforma] Generando sacudida. Ciclo:" << *ciclos << " Offset:" << offset;
        setX(x() + offset);
        (*ciclos)++;
    });

    sacudida->start(40);
}

// ============================================
// DETECCIÓN DE COLISIONES Y EVENTOS
// ============================================

void Plataforma::detectarColisionConGoku(QGraphicsItem* goku) {
    if (!goku) return;

    if (esFinal && collidesWithItem(goku)) {
        emit alcanzadaFinal();
    }
}

// ============================================
// GETTERS Y SETTERS
// ============================================

void Plataforma::setEsFinal(bool valor) {
    esFinal = valor;
}

TipoPlataforma Plataforma::getTipo() const {
    return tipo;
}

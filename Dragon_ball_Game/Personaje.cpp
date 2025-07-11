#include "Personaje.h"
#include <QDebug>

// ============================================
// CONSTRUCTOR
// ============================================

Personaje::Personaje(QObject *parent)
    : QObject(parent),
    QGraphicsPixmapItem(),
    velocidad(5),
    vidas(3),
    velocidadSalto(0.0f),
    gravedad(1.0f),
    enSalto(false),
    timerAnimacion(new QTimer(this))
{
    setPos(0, 0);  // Posición inicial
    connect(timerAnimacion, &QTimer::timeout, this, &Personaje::actualizarAnimacion);
}

// ============================================
// SISTEMA DE MOVIMIENTO
// ============================================

void Personaje::mover(short dx, short dy)
{
    setX(x() + dx * velocidad);
    setY(y() + dy * velocidad);
}

void Personaje::setVelocidad(float x, float y) {
    velocidadX = x;
    velocidadY = y;
}

float Personaje::getVelocidadX() const { return velocidadX; }

float Personaje::getVelocidadY() const { return velocidadY; }

float Personaje::getVelocidad() const { return velocidad; }

// ============================================
// SISTEMA DE SALTO
// ============================================

void Personaje::saltar()
{
    if (!enSalto) {
        enSalto = true;
        velocidadSalto = -15.0f;
        qDebug() << "[Personaje] Inicia salto con velocidad:" << velocidadSalto;
    }
}

void Personaje::actualizarSalto() {
    setY(y() + velocidadSalto);
    velocidadSalto += gravedad;
    if (velocidadSalto >= 10.0f) {
        enSalto = false;
        velocidadSalto = -10.0f;
    }
}

bool Personaje::estaSaltando() const { return enSalto; }

void Personaje::setEnSalto(bool estado) { enSalto = estado; }

float Personaje::getVelocidadSalto() const { return velocidadSalto; }

void Personaje::setVelocidadSalto(float nuevaVelocidadSalto) { velocidadSalto = nuevaVelocidadSalto; }

// ============================================
// SISTEMA DE VIDAS Y DAÑO
// ============================================

void Personaje::recibirDano()
{
    vidas--;
    qDebug() << "[Personaje] Daño recibido. Vidas restantes:" << vidas;
    if (vidas <= 0) {
        qDebug() << "[Personaje] Eliminado";
    }
}

short Personaje::getVidas() const { return vidas; }

void Personaje::setVidas(short nuevasVidas) { vidas = nuevasVidas; }

// ============================================
// SISTEMA DE ANIMACIÓN
// ============================================

void Personaje::iniciarAnimacion()
{
    indiceAnimacion = 0;
    timerAnimacion->start(100);
}

void Personaje::detenerAnimaciones() {
    qDebug() << "[Personaje] detenerAnimaciones() llamado";
    if (timerAnimacion && timerAnimacion->isActive()) {
        timerAnimacion->stop();
        qDebug() << "[Personaje] Timer de animación detenido.";
    } else {
        qDebug() << "[Personaje] Timer ya estaba detenido.";
    }
    indiceAnimacion = 0;
}

void Personaje::cambiarEstado(EstadoAnimacion nuevoEstado)
{
    if (estadoActual != nuevoEstado) {
        estadoActual = nuevoEstado;
        detenerAnimaciones();
        iniciarAnimacion();
    }
}

void Personaje::actualizarAnimacion()
{
    // Este método debe ser sobreescrito por la subclase (ej. Goku)
    // Si no se sobreescribe, no hace nada
}

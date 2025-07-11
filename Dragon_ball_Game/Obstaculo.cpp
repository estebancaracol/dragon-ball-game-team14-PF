#include "Obstaculo.h"
#include <QDebug>
#include <QGraphicsScene>

// ============================================
// CONSTRUCTOR
// ============================================
Obstaculo::Obstaculo(TipoObstaculo tipo, float dano)
    : tipoVisual(tipo), dano(dano)
{
    QString rutaImagen;
    switch (tipo) {
    case TipoObstaculo::Objetivo:
        rutaImagen = ":/Recursos/objetos/Objetivo.png";
        break;
    case TipoObstaculo::Pez:
        rutaImagen = ":/Recursos/objetos/Tiburon.png";
        break;
    case TipoObstaculo::Escombro:
        rutaImagen = ":/Recursos/objetos/Escombro.png";
        break;
    default:
        break;
    }
    setPixmap(QPixmap(rutaImagen).scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    setZValue(2);
    setVisible(true);
    setEstado(EstadoObjetivo::Inactivo);

    // Activar física según tipo
    if (tipo == TipoObstaculo::Escombro) {
        setUsarGravedad(true);
        setVelocidad(0, 0);  // velocidad inicial en Y
    } else {
        setUsarGravedad(false);
    }

    // Para peces, los ataques inician en 3
    if (tipo == TipoObstaculo::Pez) {
        ataquesRestantes = 3;
    }
}

// ============================================
// SISTEMA DE ESTADOS
// ============================================
void Obstaculo::setEstado(EstadoObjetivo nuevoEstado) {
    m_estado = nuevoEstado;
    switch (m_estado) {
    case EstadoObjetivo::Activo:
        setOpacity(1.0);
        setGraphicsEffect(nullptr);
        break;
    case EstadoObjetivo::Inactivo:
        setOpacity(0.5);
        setGraphicsEffect(nullptr);
        break;
    case EstadoObjetivo::Entregado:
        setOpacity(0.8);
        aplicarEfectoDorado();
        break;
    case EstadoObjetivo::Parpadeando:
        aplicarEfectoDorado();
        iniciarParpadeoInterno();
        break;
    }
}

// ============================================
// SISTEMA DE EFECTOS VISUALES
// ============================================
void Obstaculo::aplicarEfectoDorado() {
    auto* efecto = new QGraphicsDropShadowEffect();
    efecto->setBlurRadius(35);
    efecto->setColor(QColor(255, 215, 0)); // Dorado
    efecto->setOffset(0);
    setGraphicsEffect(efecto);
}

void Obstaculo::iniciarParpadeoInterno() {
    int intervalo = 300;
    int duracionMs = 3000;  // 3 segundos
    int repeticiones = duracionMs / intervalo;
    QTimer* timerParpadeo = new QTimer(this);
    int* contador = new int(0);

    connect(timerParpadeo, &QTimer::timeout, this, [=]() mutable {
        setOpacity(opacity() == 1.0 ? 0.3 : 1.0);
        (*contador)++;
        if (*contador >= repeticiones) {
            timerParpadeo->stop();
            delete contador;
            timerParpadeo->deleteLater();
            setEstado(EstadoObjetivo::Inactivo);
        }
    });
    timerParpadeo->start(intervalo);
}

// ============================================
// SISTEMA DE MOVIMIENTO
// ============================================
void Obstaculo::actualizarMovimiento(Goku* goku)
{
    if (tipoVisual != TipoObstaculo::Pez || !goku) return;

    float distancia = qAbs(x() - goku->x());
    if (distancia < 200) {
        float velocidad = 4.0;
        setX(x() - velocidad);
    } else {
        setX(x() - 2.0);
    }

    // Eliminar si sale de escena
    if (x() + boundingRect().width() < 0 && scene())
        scene()->removeItem(this);
}

// ============================================
// SISTEMA DE COMBATE
// ============================================
void Obstaculo::registrarAtaque(Goku* goku)
{
    if (tipoVisual != TipoObstaculo::Pez || !goku) return;

    ataquesRealizados++;
    reducirAtaques();
    qDebug() << "[Obstaculo] Pez atacó. Ataques restantes:" << ataquesRestantes;

    if (ataquesRealizados >= 3) {
        ataquesRealizados = 0;  // reiniciar por si reaparece
        goku->setVidas(goku->getVidas() - 1);
        qDebug() << "[Obstaculo] Goku atacado por pez. Vidas restantes:" << goku->getVidas();
    }
}

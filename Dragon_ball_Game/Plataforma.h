#ifndef PLATAFORMA_H
#define PLATAFORMA_H

#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QRandomGenerator>
#include <QPropertyAnimation>

enum class TipoPlataforma {
    FIJA,
    MOVIL
};

class Plataforma : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    Plataforma(TipoPlataforma tipo, float x, float y, float ancho, float tiempo = 0.0f);

    // ========== MOVIMIENTO ==========
    void mover();
    void moverAscensorVertical();
    void cambiarMovimientoAleatorio();
    void generarSacudida();

    // ========== COLISIONES ==========
    void detectarColisionConGoku(QGraphicsItem* goku);

    // ========== GETTERS ==========
    TipoPlataforma getTipo() const;
    QTimer* getTimerMovimiento() const { return timerMovimiento; }

    // ========== SETTERS Y CONFIGURACIÓN ==========
    void setEsFinal(bool valor);

signals:
    void alcanzadaFinal();

private:
    // ========== PROPIEDADES DE PLATAFORMA ==========
    TipoPlataforma tipo;
    float ancho;
    bool esFinal = false;

    // ========== PARÁMETROS DE MOVIMIENTO ==========
    float velocidad;
    float direccion;
    bool moverEnX;

    // ========== LÍMITES DE MOVIMIENTO ==========
    float limiteX1, limiteX2;
    float limiteY1, limiteY2;

    // ========== TEMPORIZADORES ==========
    QTimer* timerMovimiento;
    QTimer* timerCambioDireccion;
};

#endif // PLATAFORMA_H

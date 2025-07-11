#ifndef OBJETOFISICO_H
#define OBJETOFISICO_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>

// ========== CLASE OBJETO FÍSICO ==========
class ObjetoFisico : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    // ========== CONSTRUCTORES Y CONFIGURACIÓN ==========
    explicit ObjetoFisico(bool esProyectil = false, QObject* parent = nullptr);
    void configurarComoProyectil();

    // ========== GESTIÓN DE VELOCIDAD ==========
    void setVelocidad(float vx, float vy);
    float getVelocidadX() const { return velocidadX; }
    float getVelocidadY() const { return velocidadY; }

    // ========== GESTIÓN DE GRAVEDAD ==========
    void setUsarGravedad(bool valor) { usarGravedad = valor; }
    bool getUsarGravedad() const { return usarGravedad; }
    void setGravedad(float valor) { gravedad = valor; }
    float getGravedad() const { return gravedad; }

    // ========== SISTEMA DE FÍSICA ==========
    void actualizarFisica();
    void verificarColisiones();

    // ========== GESTIÓN DE ESTADO ==========
    void destruir();
    bool estaDestruido() const { return destruido; }

    // ========== GESTIÓN DE TIMER ==========
    void setTimer(QTimer* timer);

private:
    // ========== ATRIBUTOS DE VELOCIDAD ==========
    float velocidadX = 0.0f;
    float velocidadY = 0.0f;

    // ========== ATRIBUTOS DE GRAVEDAD ==========
    bool usarGravedad = false;
    float gravedad = 0.5f;

    // ========== ATRIBUTOS DE ESTADO ==========
    bool destruido = false;

    // ========== ATRIBUTOS DE CONTROL ==========
    QTimer* timerMovimiento = nullptr;
};

#endif // OBJETOFISICO_H

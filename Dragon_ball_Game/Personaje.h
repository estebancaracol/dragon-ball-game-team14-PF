#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QPixmap>
#include <QTimer>

// ========== ENUMERACIONES ==========
enum EstadoAnimacion {
    Quieto,
    CaminandoDerecha,
    CaminandoIzquierda,
    NadandoDerecha,
    NadandoIzquierda,
    Lanzando
};

class Personaje : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    // ========== CONSTRUCTOR/DESTRUCTOR ==========
    explicit Personaje(QObject *parent = nullptr);
    virtual ~Personaje() = default;

    // ========== MÉTODOS VIRTUALES ==========
    virtual void cargarImagen() = 0;

    // ========== ANIMACIONES ==========
    virtual void iniciarAnimacion();
    virtual void detenerAnimaciones();
    void cambiarEstado(EstadoAnimacion nuevoEstado);

    // ========== MOVIMIENTO ==========
    void mover(short dx, short dy);
    void saltar();
    void setVelocidad(float x, float y);
    void setVelocidadSalto(float nuevaVelocidadSalto);
    void setEnSalto(bool estado);
    void setUltimaDireccion(short dir) { ultimaDireccion = dir; }

    // ========== COMBATE ==========
    void recibirDano();
    void setVidas(short nuevasVidas);

    // ========== GETTERS ==========
    short getVidas() const;
    float getVelocidad() const;
    float getVelocidadSalto() const;
    bool estaSaltando() const;
    float getVelocidadX() const;
    float getVelocidadY() const;
    short getUltimaDireccion() const { return ultimaDireccion; }

protected slots:
    // ========== SLOTS ==========
    void actualizarSalto();
    virtual void actualizarAnimacion();

protected:
    // ========== PROPIEDADES DE MOVIMIENTO ==========
    short velocidad;
    float posX;
    float posY;
    bool enSalto;
    float velocidadSalto;
    float gravedad;
    float velocidadX = 0;
    float velocidadY = 0;
    short ultimaDireccion = 1; // 1 = derecha, -1 = izquierda

    // ========== PROPIEDADES DE ANIMACIÓN ==========
    EstadoAnimacion estadoActual = Quieto;
    int indiceAnimacion = 0;
    QTimer* timerAnimacion;

    // ========== PROPIEDADES DE COMBATE ==========
    short vidas;
};

#endif // PERSONAJE_H

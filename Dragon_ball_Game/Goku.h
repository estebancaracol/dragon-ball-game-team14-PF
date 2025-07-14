#ifndef GOKU_H
#define GOKU_H

#include "Personaje.h"
#include "Item.h"
#include "Caparazon.h"
#include "Fisica.h"
#include <QList>
#include <QTimer>
#include <QSet>

class Goku : public Personaje
{
    Q_OBJECT

public:
    explicit Goku(QObject *parent = nullptr);
    void cargarImagen() override;

    // ========== MOVIMIENTO ==========
    void saltar();
    void nadar(float dx, float dy);
    void activarNado();
    void desactivarNado();
    bool estaNadando() const;
    void procesarMovimiento();

    // ========== ANIMACIONES ==========
    void actualizarAnimacion() override;
    void actualizarSpriteMovimiento(EstadoAnimacion estado);
    void volverASpriteDefault();
    void iniciarAnimacionLanzamiento();
    void finalizarAnimacionLanzamiento();

    // ========== CONTROL DE TECLADO ==========
    void agregarTeclaPresionada(int key);
    void removerTeclaPresionada(int key);
    QList<int> getTeclasPresionadas() const;

    // ========== DISPARO Y TRAYECTORIA ==========
    void disparar(bool parabolico);
    void mostrarTrayectoria(bool parabolico);
    void limpiarTrayectoria();
    void disminuirAnguloTiro();
    void aumentarAnguloTiro();
    void disminuirFuerzaTiro();
    void aumentarFuerzaTiro();
    bool trayectoriaEstaVisible() const;
    void actualizarTrayectoria(bool parabolico);
    void toggleTrayectoria(bool parabolico);

    // ========== ITEMS Y RECOLECCIÓN ==========
    void recolectar(Item* item);
    void aumentarTiros(int cantidad);
    void marcarCajaLecheObligatoria();

    // ========== INVULNERABILIDAD ==========
    void activarInvulnerabilidad(int ms = 3000);
    bool getInvulnerable() const { return invulnerable; }

    // ========== GETTERS ==========
    int getTirosDisponibles() const;
    Caparazon& getCaparazon();
    float getVelocidad() const;
    float getAnguloTiro() const;
    float getFuerzaTiro() const;
    int getEsferasRecolectadas() const;
    int getBurbujasRecolectadas() const;
    bool tieneCajaLecheObligatoria() const;
    EstadoAnimacion getEstadoAnimacion() const;

    // ========== SETTERS Y CONFIGURACIÓN ==========
    void setProgreso(int esferas, int tiros, bool tieneCajaLeche);

private:
    // ========== MÉTODOS PRIVADOS ==========
    void cargarSprites();
    void procesarEstado();
    EstadoAnimacion determinarEstado();
    void cambiarEstado(EstadoAnimacion nuevoEstado);
    const QVector<QPixmap>* obtenerSpritesDelEstado(EstadoAnimacion estado);

    // ========== COMPONENTES PRINCIPALES ==========
    Caparazon caparazon;
    Fisica fisica;

    // ========== TEMPORIZADORES ==========
    QTimer* timerAnimacion;
    QTimer* timerSpriteDefault;
    QTimer* timerInvulnerabilidad = nullptr;

    // ========== SPRITES ==========
    QPixmap spriteDefault;
    QVector<QPixmap> spritesCaminandoDerecha;
    QVector<QPixmap> spritesCaminandoIzquierda;
    QVector<QPixmap> spritesNadandoDerecha;
    QVector<QPixmap> spritesNadandoIzquierda;
    QVector<QPixmap> spritesLanzando;

    // ========== ESTADO DEL PERSONAJE ==========
    short tirosDisponibles;
    bool cajaLecheObligatoriaRecolectada = false;
    bool enModoNado = false;
    bool invulnerable = false;
    bool enMovimiento = false;
    bool trayectoriaActiva = false;
    int ultimaDireccion = 1; // 1 derecha, -1 izquierda

    // ========== CONTADORES ==========
    int burbujasRecolectadas = 0;
    int esferasRecolectadas = 0;

    // ========== CONTROL DE ANIMACIÓN ==========
    int indiceAnimacionActual = 0;
    EstadoAnimacion estadoActual = Quieto;
    EstadoAnimacion estadoAnterior = Quieto;

    // ========== CONTROL DE ENTRADA ==========
    QSet<int> teclasPresionadas;

    // ========== PARÁMETROS DE DISPARO ==========
    float anguloTiro = 45.0f;
    float fuerzaTiro = 15.0f;

    // ========== TRAYECTORIA ==========
    QList<QGraphicsEllipseItem*> trayectoriaPreview;
    QPointF posicionBaseTrayectoria;
    bool trayectoriaVisible = false;  // Bandera para controlar el estado de la trayectoria
};

#endif // GOKU_H

#ifndef NIVEL_H
#define NIVEL_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include "Personaje.h"
#include "Goku.h"
#include "Fisica.h"

// ========== ENUMERACIONES ==========
enum class TipoNivel { UNO, DOS, TRES };

class Nivel : public QGraphicsScene
{
    Q_OBJECT

public:
    // ========== CONSTRUCTOR/DESTRUCTOR ==========
    explicit Nivel(QObject *parent = nullptr);
    virtual ~Nivel() {}

    // ========== MÉTODOS VIRTUALES ==========
    virtual void cargarElementos() = 0;
    virtual void cargarFondo() = 0;
    virtual void crearPersonaje() = 0;
    virtual void agregarItems() = 0;
    virtual void reiniciarNivel() = 0;
    virtual bool verificarObjetivoCompleto() = 0;
    virtual Personaje* getPersonaje() const = 0;

    // ========== GETTERS ==========
    TipoNivel getTipo() const;

    // ========== PROPIEDADES PÚBLICAS ==========
    Goku* goku;
    QTimer* timerCaida;
    float velocidadCaida;
    QTimer* timerFisica;
    Fisica fisica;
    float limiteInferiorY = 600;

signals:
    // ========== SEÑALES ==========
    void regresarAlMenu();
    void objetivoCumplido();
    void vidasAgotadas();
    void pausarTiempo();
    void reinicioCompletado();
    void objetivoEntregado(int objetivosEntregados);

protected:
    // ========== PROPIEDADES PROTEGIDAS ==========
    QGraphicsPixmapItem* fondo;
    TipoNivel tipo;
};

#endif // NIVEL_H

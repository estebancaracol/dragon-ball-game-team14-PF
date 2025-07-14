#ifndef NIVEL1_H
#define NIVEL1_H

#include "Nivel.h"
#include "Fisica.h"
#include "Goku.h"
#include "Personaje.h"
#include <QTimer>

class Nivel1 : public Nivel
{
    Q_OBJECT

public:
    // ========== CONSTRUCTOR ==========
    explicit Nivel1(QObject *parent = nullptr);

    // ========== MÉTODOS PÚBLICOS ==========
    Personaje* getPersonaje() const override;
    void detenerTodasLasPlataformas();
    void animarPlataformaFinalYMostrarVentana();
    bool estaEnPlataformaFinal() const;
    bool verificarObjetivoCompleto() override;

protected:
    // ========== MÉTODOS PROTEGIDOS (OVERRIDES) ==========
    void cargarFondo() override;
    void cargarElementos() override;
    void crearPersonaje() override;
    void agregarItems() override;
    void reiniciarNivel() override;

private:
    // ========== MÉTODOS PRIVADOS ==========
    void crearSuelosInvisibles();
    void configurarGravedad();
    void agregarPlataformas();
    void actualizarFisica();

    // ========== ELEMENTOS DEL NIVEL ==========
    QList<Plataforma*> plataformas;
    QList<QGraphicsRectItem*> suelosInvisibles;

    // ========== PERSONAJE ==========
    Goku* goku;

    // ========== FÍSICA Y MOVIMIENTO ==========
    QTimer* timerCaida;
    float velocidadCaida;
    QTimer* timerFisica;
    Fisica fisica;
    float limiteInferiorY = 600;

    // ========== GESTIÓN DE OBJETIVOS ==========
    bool objetivoCompletado = false;
    bool plataformasDetenidas = false;
};

#endif // NIVEL1_H

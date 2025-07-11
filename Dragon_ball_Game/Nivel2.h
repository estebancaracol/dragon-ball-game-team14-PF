#ifndef NIVEL2_H
#define NIVEL2_H

#include "Nivel.h"
#include "Personaje.h"
#include "Goku.h"
#include "Obstaculo.h"

class Nivel2 : public Nivel
{
    Q_OBJECT

public:
    // ========== CONSTRUCTOR ==========
    explicit Nivel2(QObject *parent = nullptr);

    // ========== MÉTODOS PÚBLICOS ==========
    Personaje* getPersonaje() const override;
    int getCantidadObjetivosEntregados() const;
    void incrementarObjetivosEntregados();
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
    void actualizarFisica();
    void crearSuelosInvisibles();
    void agregarPlataformaAscensor();
    void agregarPlataformasMoviles();
    void agregarObjetivoEnPlataformas();
    void iniciarCambioObjetivo();

    // ========== PERSONAJE ==========
    Goku* goku;

    // ========== ELEMENTOS DEL NIVEL ==========
    QList<QGraphicsRectItem*> suelosInvisibles;
    QList<Plataforma*> plataformas;
    QList<Plataforma*> plataformasObjetivo;
    QList<Obstaculo*> objetivos;

    // ========== GESTIÓN DE OBJETIVOS ==========
    QTimer* timerCambioObjetivo;
    int objetivosEntregados = 0;
    int indiceObjetivoActivo = -1;
    bool emitidoObjetivoCumplido = false;
};

#endif // NIVEL2_H

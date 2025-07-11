#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "ObjetoFisico.h"
#include "Goku.h"
#include <QGraphicsDropShadowEffect>
#include <QTimer>

// ========== ENUMERACIONES ==========
enum class TipoObstaculo {
    Objetivo,
    Pez,
    Escombro
};

enum class EstadoObjetivo {
    Activo,
    Inactivo,
    Entregado,
    Parpadeando
};

// ========== CLASE OBSTÁCULO ==========
class Obstaculo : public ObjetoFisico
{
    Q_OBJECT

public:
    // ========== CONSTRUCTORES ==========
    Obstaculo(TipoObstaculo tipo, float dano = 0);

    // ========== GESTIÓN DE ESTADO ==========
    void setEstado(EstadoObjetivo nuevoEstado);
    EstadoObjetivo estado() const { return m_estado; }

    // ========== PROPIEDADES BÁSICAS ==========
    TipoObstaculo getTipo() const { return tipoVisual; }
    float getDano() const { return dano; }

    // ========== COMPORTAMIENTO ESPECÍFICO DE PECES ==========
    void actualizarMovimiento(Goku* goku);     // Solo usado por peces
    void registrarAtaque(Goku* goku);          // Solo peces

    // ========== SISTEMA DE ATAQUES (TIBURONES) ==========
    int getAtaquesRestantes() const { return ataquesRestantes; }
    void reducirAtaques() { if (ataquesRestantes > 0) ataquesRestantes--; }

private:
    // ========== MÉTODOS INTERNOS ==========
    void iniciarParpadeoInterno();
    void aplicarEfectoDorado();

    // ========== ATRIBUTOS BÁSICOS ==========
    TipoObstaculo tipoVisual;
    float dano = 1.0f;
    EstadoObjetivo m_estado = EstadoObjetivo::Inactivo;

    // ========== ATRIBUTOS ESPECÍFICOS DE PECES ==========
    int ataquesRestantes = 3;
    int ataquesRealizados = 0; // para peces
};

#endif // OBSTACULO_H

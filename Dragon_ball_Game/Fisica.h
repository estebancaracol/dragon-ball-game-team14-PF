#ifndef FISICA_H
#define FISICA_H

#include "Personaje.h"
#include "Caparazon.h"
#include "Plataforma.h"
#include "ObjetoFisico.h"

class Fisica
{
public:
    Fisica();

    // ========== FÍSICA DE PERSONAJES ==========
    void aplicarGravedad(Personaje* personaje);
    void aplicarResistencia(Personaje* personaje, const Caparazon& caparazon);
    void actualizarFisicaSalto(Personaje* personaje, const QList<QGraphicsItem*>& colisiones);
    void actualizarFisicaSalto(Personaje* personaje, const QList<QGraphicsItem*>& colisiones, QGraphicsRectItem* zonaAgua);

    // ========== FÍSICA DE AGUA ==========
    void aplicarFisicaAgua(Personaje* personaje, QGraphicsRectItem* zonaAgua);

    // ========== COLISIONES ==========
    bool verificarColisionSuelo(Personaje* personaje, Plataforma* plataforma);

    // ========== PROYECTILES Y TIROS ==========
    QPointF calcularTiroParabolico(float angulo, float velocidad);
    QPointF calcularTiroRecto(float velocidad);
    void lanzarProyectil(ObjetoFisico* objeto, const QPointF& velocidad, bool parabolico);

    // ========== OBJETOS FÍSICOS ==========
    void afectarObjeto(ObjetoFisico* obs);
};

#endif // FISICA_H

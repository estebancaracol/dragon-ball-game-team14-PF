#include "Nivel.h"

Nivel::Nivel(QObject *parent)
    : QGraphicsScene(parent), fondo(nullptr)
{
    setSceneRect(0, 0, 920, 570); // Tamaño base del nivel
}

TipoNivel Nivel::getTipo() const {
    return tipo;
}

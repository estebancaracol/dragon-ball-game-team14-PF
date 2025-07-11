#include "CajaLeche.h"
#include <QDebug>

CajaLeche::CajaLeche(short cantidadTiros)
    : cantidadTiros(cantidadTiros)
{
    QPixmap sprite = QPixmap(":/Recursos/objetos/burbuja.png").scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setPixmap(sprite);
    spriteOriginal = sprite; // Guardar sprite original para efectos
    // Almacenar como property para acceso externo
    setProperty("cantidadTiros", cantidadTiros);
}

void CajaLeche::efecto()
{
    qDebug() << "[CajaLeche] Se recargan" << cantidadTiros << "tiros al jugador.";
}

short CajaLeche::getCantidadTiros() const {
    return cantidadTiros;
}

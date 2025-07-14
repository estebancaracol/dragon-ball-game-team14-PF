#include "CajaLeche.h"
#include <QDebug>

CajaLeche::CajaLeche(short cantidadTiros)
    : cantidadTiros(cantidadTiros)
{
    QPixmap sprite = QPixmap(":/Recursos/objetos/CajaLeche.png").scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setPixmap(sprite);
    spriteOriginal = sprite;
    setProperty("cantidadTiros", cantidadTiros);
}

void CajaLeche::efecto()
{
    qDebug() << "[CajaLeche] Se recargan" << cantidadTiros << "tiros al jugador.";
}

short CajaLeche::getCantidadTiros() const {
    return cantidadTiros;
}

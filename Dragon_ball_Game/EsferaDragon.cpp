#include "EsferaDragon.h"
#include <QDebug>

EsferaDragon::EsferaDragon(short recolectadas, short TotalRecolectar)
    : recolectadas(recolectadas), TotalRecolectar(TotalRecolectar)
{
    setPixmap(QPixmap(":/Recursos/objetos/EdelDragon.png").scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void EsferaDragon::efecto()
{
    qDebug() << "[EsferaDragon] Se suman" << recolectadas << "esferas recolectadas por el jugador.";
    qDebug() << "[EsferaDragon] Se deben recolectar" << TotalRecolectar << "esferas para ganar.";
}

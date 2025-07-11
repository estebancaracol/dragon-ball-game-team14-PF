#include "EsferaDragon.h"
#include <QDebug>

EsferaDragon::EsferaDragon(short puntaje)
    : puntaje(puntaje)
{
    setPixmap(QPixmap(":/Recursos/objetos/EdelDragon.png").scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void EsferaDragon::efecto()
{
    qDebug() << "[EsferaDragon] Se suman" << puntaje << "puntos al jugador.";
}

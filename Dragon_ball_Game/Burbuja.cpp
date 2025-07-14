#include "Burbuja.h"
#include <QDebug>

Burbuja::Burbuja(short vidasExtra)
    : vidasExtra(vidasExtra)
{
    setPixmap(QPixmap(":/Recursos/objetos/burbuja.png").scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void Burbuja::efecto()
{
    qDebug() << "[Burbuja] Se otorgan" << vidasExtra << "vidas extra al jugador.";
}

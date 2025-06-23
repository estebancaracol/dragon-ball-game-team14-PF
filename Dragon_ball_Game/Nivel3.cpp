#include "Nivel3.h"
#include <QPixmap>

Nivel3::Nivel3(QObject *parent) : Escenario(parent) {
    cargarFondo();
}

void Nivel3::cargarFondo() {
    //fondo = new QGraphicsPixmapItem(QPixmap(":/Recursos/backgraunds/BACKGRAUND NIVEL 3.png"));

    QPixmap fondoPixmap(":/Recursos/backgraunds/BACKGRAUND NIVEL 3.png");

    if (!fondoPixmap.isNull()) {
        // Escala la imagen al tamaño deseado (ajusta el tamaño a tu escena)
        fondoPixmap = fondoPixmap.scaled(920, 570, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        fondo = new QGraphicsPixmapItem(fondoPixmap);
        addItem(fondo);
    } else {
        qDebug() << "No se pudo cargar la imagen de fondo";
    }
}

#include "Niveles.h"

#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QTimer>

Escenario::Escenario(QObject *parent)
    : QGraphicsScene(parent), fondo(nullptr)
{
    qDebug() << "[Escenario] Constructor - Escena creada";
    setSceneRect(0, 0, 920, 570); // Dimensiones base
}

void Escenario::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "[Escenario] Tecla presionada:" << event->key();
    if (event->key() == Qt::Key_Escape) {
        qDebug() << "[Escenario] Tecla ESC detectada - Emitiendo señal de regreso al menú";
        emit regresarAlMenu();
    }
    QGraphicsScene::keyPressEvent(event);
}

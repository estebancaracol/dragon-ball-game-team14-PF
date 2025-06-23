#ifndef NIVELES_H
#define NIVELES_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QDebug>
//#include "Goku.h"

class Escenario : public QGraphicsScene {
    Q_OBJECT

public:
    explicit Escenario(QObject *parent = nullptr);
    virtual void cargarFondo() = 0; // Método puro para que cada nivel lo implemente

protected:
    void keyPressEvent(QKeyEvent *event) override;  // Manejar eventos de teclado
    QGraphicsPixmapItem *fondo;

signals:
    void regresarAlMenu();  // ← señal de regreso

private:
    //Goku* goku;
    //QTimer* timerFisica;
};

#endif // NIVELES_H

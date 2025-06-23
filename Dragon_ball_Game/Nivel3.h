#ifndef NIVEL3_H
#define NIVEL3_H

#include "Niveles.h"

class Nivel3 : public Escenario {
    Q_OBJECT
public:
    Nivel3(QObject *parent = nullptr);
    void cargarFondo() override;
};

#endif // NIVEL3_H

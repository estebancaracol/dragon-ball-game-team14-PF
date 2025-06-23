#ifndef NIVEL1_H
#define NIVEL1_H

#include "Niveles.h"

class Nivel1 : public Escenario {
    Q_OBJECT
public:
    Nivel1(QObject *parent = nullptr);
    void cargarFondo() override;
    void agregarPersonajes();
};

#endif // NIVEL1_H

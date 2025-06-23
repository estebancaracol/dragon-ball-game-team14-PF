#ifndef NIVEL2_H
#define NIVEL2_H

#include "Niveles.h"

class Nivel2 : public Escenario {
    Q_OBJECT
public:
    Nivel2(QObject *parent = nullptr);
    void cargarFondo() override;
};

#endif // NIVEL2_H

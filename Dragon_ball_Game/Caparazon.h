#ifndef CAPARAZON_H
#define CAPARAZON_H

class Caparazon
{
public:
    Caparazon();

    void actualizarPeso(short nuevoNivel);

    short getNivelPeso() const;
    float getFactorVelocidad() const;  // afecta velocidad de Goku
    float getFactorSalto() const;      // opcional: afecta altura de salto
    float getFactorFlotacion() const;  // para física de fluidos

private:
    short nivelPeso;         // 1 = liviano, 2 = medio, 3 = pesado
    float factorVelocidad;
    float factorSalto;
    float factorFlotacion;
};

#endif // CAPARAZON_H

#include "Caparazon.h"

Caparazon::Caparazon()
{
    actualizarPeso(1);  // Iniciar con nivel más bajo
}

void Caparazon::actualizarPeso(short nuevoNivel)
{
    nivelPeso = nuevoNivel;

    switch (nivelPeso) {
    case 1: // Liviano
        factorVelocidad = 1.0f;
        factorSalto = 1.0f;
        factorFlotacion = 1.0f;
        break;
    case 2: // Medio
        factorVelocidad = 0.8f;
        factorSalto = 0.85f;
        factorFlotacion = 0.7f;
        break;
    case 3: // Pesado
        factorVelocidad = 0.6f;
        factorSalto = 0.6f;
        factorFlotacion = 0.4f;
        break;
    default:
        factorVelocidad = 1.0f;
        factorSalto = 1.0f;
        factorFlotacion = 1.0f;
    }
}

short Caparazon::getNivelPeso() const {
    return nivelPeso;
}

float Caparazon::getFactorVelocidad() const {
    return factorVelocidad;
}

float Caparazon::getFactorSalto() const {
    return factorSalto;
}

float Caparazon::getFactorFlotacion() const {
    return factorFlotacion;
}

#ifndef NIVEL3_H
#define NIVEL3_H

#include "Nivel.h"
#include "Goku.h"
#include "Plataforma.h"
#include "Burbuja.h"
#include "Obstaculo.h"
#include "EsferaDragon.h"
#include <QGraphicsRectItem>
#include <QTimer>

class Nivel3 : public Nivel
{
    Q_OBJECT

public:
    // ========== CONSTRUCTOR ==========
    explicit Nivel3(QObject *parent = nullptr);

    // ========== MÉTODOS PÚBLICOS ==========
    Personaje* getPersonaje() const override;
    bool verificarObjetivoCompleto() override;

protected:
    // ========== MÉTODOS PROTEGIDOS (OVERRIDES) ==========
    void cargarFondo() override;
    void cargarElementos() override;
    void crearPersonaje() override;
    void agregarItems() override;
    void reiniciarNivel() override;

private:
    // ========== MÉTODOS PRIVADOS - CREACIÓN DE ELEMENTOS ==========
    void crearZonaAgua();
    void agregarObstaculos();
    void agregarPlataformaFinal();

    // ========== MÉTODOS PRIVADOS - FÍSICA Y COLISIONES ==========
    void actualizarFisica();
    void aplicarFisicaBasica();
    void manejarColisiones();
    void verificarCaidaFueraDelLimite();
    void manejarColisionConObstaculo(Obstaculo* obs);

    // ========== MÉTODOS PRIVADOS - LÓGICA DE JUEGO ==========
    void procesarVictoriaSiCorresponde();
    QPointF puntoAleatorioEnRect(const QRectF& rect);

    // ========== PERSONAJE ==========
    Goku* goku = nullptr;

    // ========== ELEMENTOS DEL NIVEL ==========
    QGraphicsRectItem* zonaAgua = nullptr;
    QList<Burbuja*> burbujas;
    QList<EsferaDragon*> esferas;
    QList<Obstaculo*> escombros;
    QList<Obstaculo*> peces;
    Plataforma* plataformaSalida = nullptr;

    // ========== TIMERS ==========
    QTimer* timerFisica = nullptr;
    QTimer* timerObstaculos = nullptr;

    // ========== CONTROL DE ESTADO ==========
    bool emitidoObjetivoCumplido = false;
};

#endif // NIVEL3_H

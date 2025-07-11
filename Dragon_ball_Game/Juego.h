#ifndef JUEGO_H
#define JUEGO_H

#include <QWidget>
#include <QStackedLayout>
#include <QGraphicsView>
#include <QMainWindow>
#include "VentanaMenuPrincipal.h"
#include "PanelInformacion.h"
#include "VentanaEmergente.h"
#include "Nivel.h"

class Juego : public QWidget
{
    Q_OBJECT

public:
    explicit Juego(QWidget *parent = nullptr);

    // ========== GESTIÓN DE NIVELES ==========
    void cargarNivel(TipoNivel tipo);
    void volverAlMenu();

    // ========== INTERFAZ DE USUARIO ==========
    void actualizarPanelInfo();

    // ========== ESTADO DEL JUEGO ==========
    bool isGameOver() const { return gameOver; }
    void setGameOver(bool state) { gameOver = state; }
    bool getModoHistoria() const { return modoHistoria; }

protected:
    // ========== EVENTOS DE ENTRADA ==========
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    // ========== NAVEGACIÓN DE NIVELES ==========
    void mostrarNivel1();
    void mostrarNivel2();
    void mostrarNivel3();
    void iniciarJuegoCompleto();
    void mostrarTransicionNivel();

    // ========== GESTIÓN DE GAME OVER ==========
    void mostrarGameOver();
    void verificarGameOver();

private:
    // ========== MÉTODOS PRIVADOS ==========
    void conectarSenales();
    void eliminarNivelActual();
    void mostrarVentanaEmergente(VentanaEmergente::TipoMensaje tipo, int siguienteNivel = -1);

    // ========== COMPONENTES DE INTERFAZ ==========
    VentanaMenuPrincipal* menu;
    QGraphicsView* vistaNivel;
    PanelInformacion* panelInfo;
    QStackedLayout* layoutPrincipal;
    QWidget* widgetJuego;
    QHBoxLayout* layoutJuego;

    // ========== GESTIÓN DE NIVELES ==========
    Nivel* nivelActual;

    // ========== TEMPORIZADORES ==========
    QTimer* timerActualizacionPanel;

    // ========== PROGRESO DEL JUEGO ==========
    int esferasAcumuladas = 0;
    int esferasHistoricasNivelActual = 0;
    int tirosAcumulados = 0;
    bool cajaLecheObligatoriaObtenida = false;

    // ========== ESTADO DEL JUEGO ==========
    bool modoHistoria = false;
    bool gameOver = false;
};

#endif // JUEGO_H

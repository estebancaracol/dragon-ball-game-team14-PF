#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>

#include "VentanaJuego.h"
#include "Nivel1.h"
#include "Nivel2.h"
#include "Nivel3.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void cargarNiveles();

    void iniciarNivel1();
    void iniciarNivel2();
    void iniciarNivel3();

private slots:
    void volverAlMenu();  // Nuevo slot para manejar el regreso al menú

private:
    Ui::MainWindow *ui;
    QGraphicsView *vista;
    VentanaJuego *ventanaJuego;
    Escenario *escenarioActual;

};
#endif // MAINWINDOW_H

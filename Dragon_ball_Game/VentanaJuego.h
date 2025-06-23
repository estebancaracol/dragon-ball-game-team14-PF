#ifndef VENTANAJUEGO_H
#define VENTANAJUEGO_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QTimer>
#include <QSoundEffect>

class VentanaJuego : public QWidget
{
    Q_OBJECT

public:
    explicit VentanaJuego(QWidget *parent = nullptr);

signals:
    void iniciarJuegoCompleto();
    void mostrarNivel1();
    void mostrarNivel2();
    void mostrarNivel3();
    void verRecords();

private slots:
    void cambiarColorMarco();

private:
    QPushButton *botonJugar;
    QPushButton *botonNivel1;
    QPushButton *botonNivel2;
    QPushButton *botonNivel3;
    QPushButton *botonRecords;

    QLabel *logoLabel;
    QFrame *marcoLogo;
    QTimer *timerColor;
    QSoundEffect *efectoBoton;
    int colorIndex;
};

#endif // VENTANAJUEGO_H

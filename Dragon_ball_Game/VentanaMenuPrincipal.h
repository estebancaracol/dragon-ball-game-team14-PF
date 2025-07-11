#ifndef VENTANAMENUPRINCIPAL_H
#define VENTANAMENUPRINCIPAL_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QTimer>
#include <QSoundEffect>

// ========== CLASE VENTANA MENÚ PRINCIPAL ==========
class VentanaMenuPrincipal : public QWidget
{
    Q_OBJECT

public:
    // ========== CONSTRUCTORES ==========
    explicit VentanaMenuPrincipal(QWidget *parent = nullptr);

signals:
    // ========== SEÑALES DE NAVEGACIÓN ==========
    void iniciarJuegoCompleto();  // Modo historia
    void mostrarNivel1();
    void mostrarNivel2();
    void mostrarNivel3();
    //void verRecords();

private slots:
    // ========== SLOTS INTERNOS ==========
    void cambiarColorMarco();

private:
    // ========== COMPONENTES DE INTERFAZ ==========
    QPushButton *botonJugar;
    QPushButton *botonNivel1;
    QPushButton *botonNivel2;
    QPushButton *botonNivel3;
    //QPushButton *botonRecords;

    QLabel *logoLabel;
    QFrame *marcoLogo;

    void configurarEstilos(const QString &fontFamily);

    // ========== SISTEMA DE EFECTOS ==========
    QTimer *timerColor;
    QSoundEffect *efectoBoton;

    // ========== CONTROL DE ANIMACIONES ==========
    int colorIndex;
};

#endif // VENTANAMENUPRINCIPAL_H

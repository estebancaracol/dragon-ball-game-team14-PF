#ifndef VENTANAEMERGENTE_H
#define VENTANAEMERGENTE_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFontDatabase>

// ========== CLASE VENTANA EMERGENTE ==========
class VentanaEmergente : public QDialog
{
    Q_OBJECT

public:
    // ========== ENUMERACIONES ==========
    enum TipoMensaje {
        GAME_OVER,
        NIVEL_COMPLETADO,
        JUEGO_COMPLETADO
    };

    // ========== CONSTRUCTORES ==========
    explicit VentanaEmergente(TipoMensaje tipo, QWidget *parent = nullptr);
    void forzarModoReintentar();

signals:
    // ========== SEÑALES ==========
    void volverAlMenu();
    void siguienteNivel();
    void reintentarNivel();

private:
    // ========== MÉTODOS INTERNOS ==========
    void configurarEstilo();

protected:
    // ========== MANEJO DE EVENTOS ==========
    void keyPressEvent(QKeyEvent *event);
};

#endif // VENTANAEMERGENTE_H

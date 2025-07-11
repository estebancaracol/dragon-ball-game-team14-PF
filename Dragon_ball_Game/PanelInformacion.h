#ifndef PANELINFORMACION_H
#define PANELINFORMACION_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QFont>

// ========== FORWARD DECLARATIONS ==========
class Goku;

// ========== CLASE PANEL INFORMACIÓN ==========
class PanelInformacion : public QWidget
{
    Q_OBJECT

public:
    // ========== CONSTRUCTORES ==========
    explicit PanelInformacion(QWidget *parent = nullptr);

    // ========== ACTUALIZACIÓN DE INFORMACIÓN ==========
    void actualizarInformacion(Goku* goku, int nivelActual, bool modoHistoria = false, int objetivosEntregados = 0);

    // ========== GESTIÓN DE TIEMPO ==========
    void iniciarTiempo();
    void pausarTiempo();
    void reiniciarTiempo();
    int getTiempoRestante() const;

private slots:
    // ========== SLOTS INTERNOS ==========
    void actualizarTiempo();

signals:
    // ========== SEÑALES ==========
    void tiempoAgotado();

private:
    // ========== MÉTODOS INTERNOS ==========
    void configurarEstilo();
    QString formatearTiempo(int segundos);

    // ========== COMPONENTES DE INTERFAZ ==========
    QLabel* labelTitulo;
    QLabel* labelTiempo;
    QLabel* labelNivel;
    QLabel* labelVidas;
    QLabel* labelEsferas;
    QLabel* labelTiros;
    QLabel* labelObjetivos;
    QVBoxLayout* layout;

    // ========== SISTEMA DE TIEMPO ==========
    QTimer* timerTiempo;
    int tiempoRestante; // en segundos (3 minutos = 180 segundos)

    // ========== CONSTANTES ==========
    static const int TIEMPO_NIVEL = 180; // 3 minutos por nivel
};

#endif // PANELINFORMACION_H

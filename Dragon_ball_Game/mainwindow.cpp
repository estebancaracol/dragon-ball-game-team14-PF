#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ventanaJuego = new VentanaJuego();
    setCentralWidget(ventanaJuego);
    setFixedSize(920, 570); // Tamaño del menú

    connect(ventanaJuego, &VentanaJuego::iniciarJuegoCompleto, this, &MainWindow::cargarNiveles);
    connect(ventanaJuego, &VentanaJuego::mostrarNivel1, this, &MainWindow::iniciarNivel1);
    connect(ventanaJuego, &VentanaJuego::mostrarNivel2, this, &MainWindow::iniciarNivel2);
    connect(ventanaJuego, &VentanaJuego::mostrarNivel3, this, &MainWindow::iniciarNivel3);
    connect(ventanaJuego, &VentanaJuego::verRecords, this, []() {
        // Aquí más adelante puedes abrir una ventana con los récords
        qDebug() << "[MainWindow] Mostrar récords (a implementar)";
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::cargarNiveles(){
    iniciarNivel1();
    //iniciarNivel2();
    //iniciarNivel3();
}

void MainWindow::iniciarNivel1() {
    qDebug() << "[MainWindow] Iniciando Nivel 1";
    escenarioActual = new Nivel1(this);
    vista = new QGraphicsView(escenarioActual, this);
    vista->setFocusPolicy(Qt::StrongFocus);
    vista->setFocus();
    vista->setFixedSize(920, 570);
    this->setFixedSize(920, 570);

    takeCentralWidget();
    setCentralWidget(vista);

    // Conectar señal de regreso al menú
    connect(escenarioActual, &Escenario::regresarAlMenu, this, &MainWindow::volverAlMenu);
    qDebug() << "[MainWindow] Nivel 1 listo - Presiona ESC para regresar";
}

void MainWindow::iniciarNivel2() {
    qDebug() << "[MainWindow] Iniciando Nivel 2";
    escenarioActual = new Nivel2(this);
    vista = new QGraphicsView(escenarioActual, this);
    vista->setFocusPolicy(Qt::StrongFocus);
    vista->setFocus();
    vista->setFixedSize(920, 570);
    this->setFixedSize(920, 570);

    takeCentralWidget();
    setCentralWidget(vista);

    // Conectar señal de regreso al menú
    connect(escenarioActual, &Escenario::regresarAlMenu, this, &MainWindow::volverAlMenu);
    qDebug() << "[MainWindow] Nivel 2 listo - Presiona ESC para regresar";
}

void MainWindow::iniciarNivel3() {
    qDebug() << "[MainWindow] Iniciando Nivel 3";
    escenarioActual = new Nivel3(this);
    vista = new QGraphicsView(escenarioActual, this);
    vista->setFocusPolicy(Qt::StrongFocus);
    vista->setFocus();
    vista->setFixedSize(920, 570);
    this->setFixedSize(920, 570);

    takeCentralWidget();
    setCentralWidget(vista);

    // Conectar señal de regreso al menú
    connect(escenarioActual, &Escenario::regresarAlMenu, this, &MainWindow::volverAlMenu);
    qDebug() << "[MainWindow] Nivel 3 listo - Presiona ESC para regresar";
}

void MainWindow::volverAlMenu()
{
    qDebug() << "[MainWindow] Regresando al menú principal";

    if (vista) {
        qDebug() << "[MainWindow] Limpiando vista";
        vista->setScene(nullptr);
        delete vista;
        vista = nullptr;
    }

    if (escenarioActual) {
        qDebug() << "[MainWindow] Eliminando escenario actual";
        delete escenarioActual;
        escenarioActual = nullptr;
    }

    qDebug() << "[MainWindow] Restaurando ventana de juego principal";

    if (!ventanaJuego) {
        qDebug() << "[Error] ventanaJuego es nullptr. No se puede restaurar el menú.";
        return;
    }

    //Esta operación era riesgosa antes, pero ahora será segura
    setCentralWidget(ventanaJuego);
    ventanaJuego->setFocus();
}


#include "VentanaJuego.h"
#include <QPixmap>
#include <QStringList>
#include <QFontDatabase>
#include <QSoundEffect>

VentanaJuego::VentanaJuego(QWidget *parent)
    : QWidget(parent), colorIndex(0)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);

    // --- CARGAR FUENTE PERSONALIZADA ---
    int id = QFontDatabase::addApplicationFont(":/Recursos/fuente/Saiyan-Sans.ttf");
    QString fontFamily = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont fuenteDragonBall(fontFamily);
    fuenteDragonBall.setPointSize(18);

    // --- SONIDO DE BOTON ---
    // efectoBoton = new QSoundEffect(this);
    // efectoBoton->setSource(QUrl("qrc:/Recursos/sonidos/dbz_button_click.wav"));
    // efectoBoton->setVolume(0.8);

    // --- LOGO ---
    logoLabel = new QLabel(this);
    QPixmap logoPixmap(":/Recursos/titulo/dragonball_logo.png");
    logoLabel->setPixmap(logoPixmap.scaled(300, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setAlignment(Qt::AlignCenter);

    marcoLogo = new QFrame(this);
    marcoLogo->setFrameStyle(QFrame::Box);
    marcoLogo->setLineWidth(4);
    marcoLogo->setStyleSheet("border: 4px solid red; border-radius: 10px; background-color: white;");
    QVBoxLayout *marcoLayout = new QVBoxLayout(marcoLogo);
    marcoLayout->addWidget(logoLabel);
    layout->addWidget(marcoLogo, 0, Qt::AlignCenter);

    // --- BOTONES ---

    // --- BOTONES PERSONALIZADOS ---
    QString estiloBoton = QString(
                              "QPushButton {"
                              " background-color: orange;"
                              " color: black;"
                              " font-family: '%1';"
                              " font-size: 20px;"
                              " padding: 15px 0;"
                              " border: 3px solid red;"
                              " border-radius: 12px;"
                              " min-width: 400px;"
                              "}"
                              "QPushButton:hover {"
                              " background-color: #ffcc00;"
                              " border: 3px solid darkred;"
                              "}"
                              ).arg(fontFamily);

    botonJugar = new QPushButton("Iniciar Juego", this);
    botonNivel1 = new QPushButton("Nivel UNO", this);
    botonNivel2 = new QPushButton("Nivel DOS", this);
    botonNivel3 = new QPushButton("Nivel TRES", this);
    botonRecords = new QPushButton("Ver Records", this);

    QList<QPushButton*> botones = {botonJugar, botonNivel1, botonNivel2, botonNivel3, botonRecords};
    for (QPushButton* boton : botones) {
        boton->setStyleSheet(estiloBoton);
        boton->setFont(fuenteDragonBall);
        layout->addWidget(boton, 0, Qt::AlignHCenter);

        // Conectar sonido
        // connect(boton, &QPushButton::clicked, this, [this]() {
        //     efectoBoton->play();
        // });
    }

    // --- CONEXIONES DE NAVEGACION ---
    connect(botonJugar, &QPushButton::clicked, this, &VentanaJuego::iniciarJuegoCompleto);
    connect(botonNivel1, &QPushButton::clicked, this, &VentanaJuego::mostrarNivel1);
    connect(botonNivel2, &QPushButton::clicked, this, &VentanaJuego::mostrarNivel2);
    connect(botonNivel3, &QPushButton::clicked, this, &VentanaJuego::mostrarNivel3);
    connect(botonRecords, &QPushButton::clicked, this, &VentanaJuego::verRecords);

    // --- ANIMACION DE COLOR DEL MARCO ---
    timerColor = new QTimer(this);
    connect(timerColor, &QTimer::timeout, this, &VentanaJuego::cambiarColorMarco);
    timerColor->start(500); // cambia cada medio segundo
}

void VentanaJuego::cambiarColorMarco()
{
    QStringList colores = {"red", "blue", "orange", "purple", "green"};
    QString colorActual = colores[colorIndex % colores.size()];
    marcoLogo->setStyleSheet(QString("border: 4px solid %1; border-radius: 10px; background-color: white;").arg(colorActual));
    colorIndex++;
}


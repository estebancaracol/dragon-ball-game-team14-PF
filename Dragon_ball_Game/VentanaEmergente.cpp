#include "VentanaEmergente.h"

#include <QGraphicsDropShadowEffect>
#include <QPainterPath>
#include <QKeyEvent>

// ============================================
// CONSTRUCTOR Y CONFIGURACIÓN INICIAL
// ============================================

VentanaEmergente::VentanaEmergente(TipoMensaje tipo, QWidget *parent)
    : QDialog(parent, Qt::Dialog | Qt::FramelessWindowHint)
{
    setFixedSize(700, 400);
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);

    // Aplicar bordes redondeados reales a la ventana
    QPainterPath path;
    path.addRoundedRect(rect(), 20, 20);
    QRegion region(path.toFillPolygon().toPolygon());
    setMask(region);

    // Layout principal
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *fondo = new QLabel(this);
    QString rutaFondo;

    switch (tipo) {
    case GAME_OVER:
        rutaFondo = ":/Recursos/backgraunds/img_game_over.png";
        break;
    case NIVEL_COMPLETADO:
    case JUEGO_COMPLETADO:
        rutaFondo = ":/Recursos/backgraunds/fondo_emergente.png";
        break;
    }

    fondo->setPixmap(QPixmap(rutaFondo).scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    fondo->setFixedSize(size());
    fondo->lower();

    // Layout de contenido sobre el fondo
    QVBoxLayout *contenidoLayout = new QVBoxLayout();
    contenidoLayout->setContentsMargins(420, 170, 40, 40);
    contenidoLayout->setSpacing(15);

    // Mensaje central
    QLabel *mensajeLabel = new QLabel(this);
    mensajeLabel->setAlignment(Qt::AlignCenter);
    mensajeLabel->setWordWrap(true);
    mensajeLabel->setObjectName("mensajeLabel");

    // Sombra real
    QGraphicsDropShadowEffect *sombra = new QGraphicsDropShadowEffect(this);
    sombra->setBlurRadius(15);
    sombra->setOffset(0, 0);
    sombra->setColor(Qt::white);
    mensajeLabel->setGraphicsEffect(sombra);

    // Botones
    QPushButton *botonAccion = new QPushButton(this);
    botonAccion->setFixedHeight(45);

    QPushButton *botonReintentar = new QPushButton("Reintentar Nivel", this);
    botonReintentar->setFixedHeight(45);
    botonReintentar->hide();

    // Texto según tipo
    switch (tipo) {
    case GAME_OVER:
        mensajeLabel->setText("¡GAME OVER!");
        botonAccion->setText("Volver al Menú");
        botonReintentar->show();
        break;
    case NIVEL_COMPLETADO:
        mensajeLabel->setText("¡Nivel Completado!");
        botonAccion->setText("Siguiente Nivel");
        break;
    case JUEGO_COMPLETADO:
        mensajeLabel->setText("¡GANASTE! Juego Terminado - ¡Has reunido las 7 Esferas del Dragon!");
        botonAccion->setText("Volver al Menú");
        break;
    }

    connect(botonAccion, &QPushButton::clicked, [=]() {
        if (tipo == GAME_OVER || tipo == JUEGO_COMPLETADO)
            emit volverAlMenu();
        else if (tipo == NIVEL_COMPLETADO)
            emit siguienteNivel();
        close();
    });

    connect(botonReintentar, &QPushButton::clicked, [=]() {
        emit reintentarNivel();
        close();
    });

    contenidoLayout->addWidget(mensajeLabel);
    contenidoLayout->addWidget(botonAccion);
    contenidoLayout->addWidget(botonReintentar);

    layout->addLayout(contenidoLayout);
    configurarEstilo();
}

// ============================================
// CONFIGURACIÓN DE ESTILOS Y EFECTOS
// ============================================

void VentanaEmergente::configurarEstilo() {
    int id = QFontDatabase::addApplicationFont(":/Recursos/fuente/Saiyan-Sans.ttf");
    QString fontFamily = QFontDatabase::applicationFontFamilies(id).at(0);

    setStyleSheet(
        "QDialog {"
        "   background-color: transparent;"
        "   border-radius: 20px;"
        "}"
        "QLabel#mensajeLabel {"
        "   color: white;"
        "   font-family: '" + fontFamily + "';"
                       "   font-size: 22px;"
                       "   padding: 10px;"
                       "   background-color: rgba(0, 0, 0, 160);"
                       "   border-radius: 12px;"
                       "   border: 2px solid white;"
                       "}"
                       "QPushButton {"
                       "   background-color: #FF5500;"
                       "   color: white;"
                       "   font-family: '" + fontFamily + "';"
                       "   font-size: 18px;"
                       "   border: 2px solid white;"
                       "   border-radius: 10px;"
                       "   padding: 6px;"
                       "}"
                       "QPushButton:hover {"
                       "   background-color: #FF8800;"
                       "   border: 2px solid yellow;"
                       "}"
        );
}

// ============================================
// FUNCIONES DE CONTROL Y COMPORTAMIENTO
// ============================================

void VentanaEmergente::forzarModoReintentar() {
    QPushButton* botonAccion = findChild<QPushButton*>();
    if (botonAccion) botonAccion->setText("Volver al Menú");
    QPushButton* botonReintentar = findChildren<QPushButton*>().value(1);
    if (botonReintentar) botonReintentar->show();

    disconnect(botonAccion, nullptr, nullptr, nullptr);
    connect(botonAccion, &QPushButton::clicked, [=]() {
        emit volverAlMenu();
        close();
    });
}

// ============================================
// MANEJO DE EVENTOS
// ============================================

void VentanaEmergente::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        qDebug() << "[VentanaEmergente] Tecla Escape ignorada.";
        return;  // Evita que cierre la ventana
    }

    QDialog::keyPressEvent(event);  // Pasa otros eventos normalmente
}

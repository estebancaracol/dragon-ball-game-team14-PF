#include <QApplication>
#include <QIcon>
#include <QDebug>
#include "AudioManager.h"

#include "Juego.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Información general de la aplicación
    a.setApplicationName("Dragon Ball Game");

    // Icono de la aplicación (asegúrate que exista en el .qrc)
    QIcon iconoApp(":/Recursos/objetos/DragonBallLogo.png");
    if (!iconoApp.isNull()) {
        a.setWindowIcon(iconoApp);
        qDebug() << "[main] Icono de aplicación asignado correctamente.";
    } else {
        qDebug() << "[main] ¡Advertencia! Icono no encontrado en la ruta proporcionada.";
    }
    // Crear y mostrar ventana principal
    Juego w;
    w.show();

    return a.exec();
}

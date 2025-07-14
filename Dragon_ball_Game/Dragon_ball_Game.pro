QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QT += multimedia

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AudioManager.cpp \
    Burbuja.cpp \
    CajaLeche.cpp \
    Caparazon.cpp \
    EsferaDragon.cpp \
    Fisica.cpp \
    Goku.cpp \
    Item.cpp \
    Juego.cpp \
    Nivel.cpp \
    Nivel1.cpp \
    Nivel2.cpp \
    Nivel3.cpp \
    ObjetoFisico.cpp \
    Obstaculo.cpp \
    PanelInformacion.cpp \
    Personaje.cpp \
    Plataforma.cpp \
    VentanaEmergente.cpp \
    VentanaMenuPrincipal.cpp \
    main.cpp

HEADERS += \
    AudioManager.h \
    Burbuja.h \
    CajaLeche.h \
    Caparazon.h \
    EsferaDragon.h \
    Fisica.h \
    Goku.h \
    Item.h \
    Juego.h \
    Nivel.h \
    Nivel1.h \
    Nivel2.h \
    Nivel3.h \
    ObjetoFisico.h \
    Obstaculo.h \
    PanelInformacion.h \
    Personaje.h \
    Plataforma.h \
    VentanaEmergente.h \
    VentanaMenuPrincipal.h

FORMS += \
    Juego.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc

#include "Juego.h"
#include "AudioManager.h"
#include "Nivel1.h"
#include "Nivel2.h"
#include "Nivel3.h"
#include "Goku.h"

#include <QKeyEvent>

// ============================================
// CONSTRUCTOR Y CONFIGURACIÓN INICIAL
// ============================================

Juego::Juego(QWidget *parent)
    : QWidget(parent), nivelActual(nullptr)
{
    layoutPrincipal = new QStackedLayout(this);

    // Crear menú principal
    menu = new VentanaMenuPrincipal(this);

    // Crear contenedor para el juego (panel + vista nivel)
    widgetJuego = new QWidget(this);
    layoutJuego = new QHBoxLayout(widgetJuego);
    layoutJuego->setContentsMargins(0, 0, 0, 0);
    layoutJuego->setSpacing(0);

    // Crear panel de información (izquierda)
    panelInfo = new PanelInformacion(widgetJuego);

    // Crear vista del nivel (centro)
    vistaNivel = new QGraphicsView(widgetJuego);
    vistaNivel->setFixedSize(920, 570);

    // Agregar elementos al layout de juego
    layoutJuego->addWidget(panelInfo);
    layoutJuego->addWidget(vistaNivel);

    // Agregar juego y menú al layout principal
    layoutPrincipal->addWidget(menu);
    layoutPrincipal->addWidget(widgetJuego);
    setLayout(layoutPrincipal);

    // Timer para actualizar la interfaz cada 100ms
    timerActualizacionPanel = new QTimer(this);
    connect(timerActualizacionPanel, &QTimer::timeout, this, &Juego::actualizarPanelInfo);

    // Conectar señales del menú
    conectarSenales();

    // Inicialmente mostrar el menú
    layoutPrincipal->setCurrentWidget(menu);

    // Configuración general
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // ===== INICIALIZAR AUDIO Y MÚSICA DE FONDO =====
    // Inicializar el sistema de audio
    AudioManager* audioManager = AudioManager::instancia();

    // Cargar y reproducir música de fondo
    audioManager->cargarMusicaFondo(":/Recursos/Audio/musica_fondo.wav");

    // Configurar volumen bajo para que no interfiera
    audioManager->setVolumenMusica(0.50f); // 50% del volumen máximo

    // Iniciar la música de fondo en loop
    audioManager->reproducirMusica(true);

    qDebug() << "[Juego] Juego inicializado con música de fondo";
}

void Juego::conectarSenales()
{
    connect(menu, &VentanaMenuPrincipal::mostrarNivel1, this, &Juego::mostrarNivel1);
    connect(menu, &VentanaMenuPrincipal::mostrarNivel2, this, &Juego::mostrarNivel2);
    connect(menu, &VentanaMenuPrincipal::mostrarNivel3, this, &Juego::mostrarNivel3);
    connect(menu, &VentanaMenuPrincipal::iniciarJuegoCompleto, this, &Juego::iniciarJuegoCompleto);
}

// ============================================
// NAVEGACIÓN ENTRE NIVELES
// ============================================

void Juego::mostrarNivel1() {
    AudioManager::instancia()->reproducirSonido(AudioManager::CLICK_MENU);
    modoHistoria = false;
    cargarNivel(TipoNivel::UNO);
}

void Juego::mostrarNivel2() {
    AudioManager::instancia()->reproducirSonido(AudioManager::CLICK_MENU);
    modoHistoria = false;
    cargarNivel(TipoNivel::DOS);
}

void Juego::mostrarNivel3() {
    AudioManager::instancia()->reproducirSonido(AudioManager::CLICK_MENU);
    modoHistoria = false;
    cargarNivel(TipoNivel::TRES);
}

void Juego::iniciarJuegoCompleto() {
    AudioManager::instancia()->reproducirSonido(AudioManager::CLICK_MENU);
    modoHistoria = true;
    cargarNivel(TipoNivel::UNO);
}

void Juego::volverAlMenu() {
    AudioManager::instancia()->reproducirSonido(AudioManager::CLICK_MENU);
    timerActualizacionPanel->stop();
    panelInfo->pausarTiempo();
    eliminarNivelActual();
    setGameOver(false);
    layoutPrincipal->setCurrentWidget(menu);

    // Reiniciar progreso historia
    esferasAcumuladas = 0;
    tirosAcumulados = 0;
    cajaLecheObligatoriaObtenida = false;
    modoHistoria = false;
}

// ============================================
// GESTIÓN DE NIVELES
// ============================================

void Juego::cargarNivel(TipoNivel tipo)
{
    // Efecto de transición
    AudioManager::instancia()->reproducirSonido(AudioManager::TRANSICION_NIVEL);

    eliminarNivelActual();

    // Crear el nivel correspondiente
    switch (tipo) {
    case TipoNivel::UNO:
        nivelActual = new Nivel1(this);
        connect(nivelActual, &Nivel::reinicioCompletado, this, [this]() {
            qDebug() << "[Juego] Reinicio completado. Game Over desactivado.";
            this->setGameOver(false);
        });
        break;
    case TipoNivel::DOS:
        nivelActual = new Nivel2(this);
        static_cast<Nivel2*>(nivelActual)->setJuego(this);
        connect(nivelActual, &Nivel::reinicioCompletado, this, [this]() {
            qDebug() << "[Juego] Reinicio completado. Game Over desactivado.";
            this->setGameOver(false);
        });
        break;
    case TipoNivel::TRES:
        nivelActual = new Nivel3(this);
        connect(nivelActual, &Nivel::reinicioCompletado, this, [this]() {
            qDebug() << "[Juego] Reinicio completado. Game Over desactivado.";
            this->setGameOver(false);
        });
        break;
    }

    if (!nivelActual) return;

    // Conectar señales
    connect(nivelActual, &Nivel::objetivoCumplido, this, &Juego::mostrarTransicionNivel);
    connect(nivelActual, &Nivel::pausarTiempo, panelInfo, &PanelInformacion::pausarTiempo);
    connect(nivelActual, &Nivel::regresarAlMenu, this, &Juego::volverAlMenu);
    connect(nivelActual, &Nivel::vidasAgotadas, this, &Juego::verificarGameOver);
    connect(panelInfo, &PanelInformacion::tiempoAgotado, this, &Juego::verificarGameOver);

    // Establecer escena y enfoque
    vistaNivel->setScene(nivelActual);
    vistaNivel->setFocus();

    // Restaurar progreso si estamos en modo historia
    Goku* goku = dynamic_cast<Goku*>(nivelActual->getPersonaje());
    if (modoHistoria) {
        if (goku) {
            goku->setProgreso(esferasAcumuladas, tirosAcumulados, cajaLecheObligatoriaObtenida);
        }
    } else {
        if (tipo == TipoNivel::DOS && goku) {
            goku->aumentarTiros(5);  // Darle 5 tiros de cortesía
            qDebug() << "[Juego] Tiros de cortesía añadidos para Nivel 2 (modo normal)";
        }
    }

    // Configurar e iniciar panel de información
    panelInfo->reiniciarTiempo();
    panelInfo->iniciarTiempo();

    // Iniciar actualización periódica del panel
    timerActualizacionPanel->start(100);

    // Cambiar a la vista del juego
    layoutPrincipal->setCurrentWidget(widgetJuego);
}

void Juego::eliminarNivelActual()
{
    if (nivelActual) {
        vistaNivel->setScene(nullptr);
        delete nivelActual;
        nivelActual = nullptr;
    }
}


// ============================================
// MANEJO DE EVENTOS DE TECLADO
// ============================================

void Juego::keyPressEvent(QKeyEvent* event) {
    qDebug() << "[KEY PRESS] Tecla presionada:" << event->key();

    if (isGameOver()) {
        qDebug() << "[KEY PRESS] Juego terminado - Ignorando";
        return;
    }

    if (!nivelActual) return;

    Goku* goku = dynamic_cast<Goku*>(nivelActual->getPersonaje());
    if (!goku) return;

    TipoNivel tipo = nivelActual->getTipo();

    // Verificar si la tecla está permitida en este nivel
    if (!esTeclaPermitidaEnNivel(event->key(), tipo)) {
        qDebug() << "[KEY PRESS] Tecla no permitida en este nivel:" << event->key();
        return;
    }

    // AGREGAR TECLA PARA MOVIMIENTO CONTINUO
    if (event->key() == Qt::Key_A || event->key() == Qt::Key_D ||
        event->key() == Qt::Key_W || event->key() == Qt::Key_S) {
        goku->agregarTeclaPresionada(event->key());
    }

    // Procesar acciones específicas por nivel
    switch (tipo) {
    case TipoNivel::UNO:
        procesarTeclasNivelUno(event->key(), goku);
        break;
    case TipoNivel::DOS:
        procesarTeclasNivelDos(event->key(), goku);
        break;
    case TipoNivel::TRES:
        procesarTeclasNivelTres(event->key(), goku);
        break;
    }

    // Tecla común para todos los niveles
    if (event->key() == Qt::Key_Escape) {
        volverAlMenu();
    }
}

void Juego::keyReleaseEvent(QKeyEvent* event) {
    // Evitar eventos repetidos
    if (event->isAutoRepeat()) {
        qDebug() << "[KEY RELEASE] Ignorando auto-repeat para tecla:" << event->key();
        return;
    }

    qDebug() << "[KEY RELEASE] Tecla liberada:" << event->key();

    if (!nivelActual || isGameOver()) return;

    Goku* goku = dynamic_cast<Goku*>(nivelActual->getPersonaje());
    if (!goku) return;

    TipoNivel tipo = nivelActual->getTipo();

    // Verificar si la tecla está permitida en este nivel
    if (!esTeclaPermitidaEnNivel(event->key(), tipo)) {
        return;
    }

    // REMOVER TECLA PARA MOVIMIENTO CONTINUO
    if (event->key() == Qt::Key_A || event->key() == Qt::Key_D ||
        event->key() == Qt::Key_W || event->key() == Qt::Key_S) {
        goku->removerTeclaPresionada(event->key());
    }
}

bool Juego::esTeclaPermitidaEnNivel(int key, TipoNivel tipo) {
    switch (tipo) {
    case TipoNivel::UNO:
        return (key == Qt::Key_A || key == Qt::Key_D || key == Qt::Key_Space || key == Qt::Key_Escape);

    case TipoNivel::DOS:
        return (key == Qt::Key_A || key == Qt::Key_D || key == Qt::Key_Z ||
                key == Qt::Key_X || key == Qt::Key_W || key == Qt::Key_S ||
                key == Qt::Key_Space || key == Qt::Key_E || key == Qt::Key_Q ||
                key == Qt::Key_C || key == Qt::Key_Escape);

    case TipoNivel::TRES:
        return (key == Qt::Key_A || key == Qt::Key_D || key == Qt::Key_W ||
                key == Qt::Key_S || key == Qt::Key_Escape);

    default:
        return false;
    }
}

void Juego::procesarTeclasNivelUno(int key, Goku* goku) {
    switch (key) {
    case Qt::Key_A:
        if (!goku->estaSaltando()) {
            float velocidad = goku->getVelocidad();
            float factorVelocidad = goku->getCaparazon().getFactorVelocidad();
            goku->setX(goku->x() - velocidad * factorVelocidad);
        }
        break;

    case Qt::Key_D:
        if (!goku->estaSaltando()) {
            float velocidad = goku->getVelocidad();
            float factorVelocidad = goku->getCaparazon().getFactorVelocidad();
            goku->setX(goku->x() + velocidad * factorVelocidad);
        }
        break;

    case Qt::Key_Space:
        if (!goku->estaSaltando() && !goku->estaNadando()) {
            AudioManager::instancia()->reproducirSonido(AudioManager::SALTO);
            goku->saltar();
        }
        break;
    }
}

void Juego::procesarTeclasNivelDos(int key, Goku* goku) {
    switch (key) {
    case Qt::Key_A:
        if (!goku->estaSaltando()) {
            float velocidad = goku->getVelocidad();
            float factorVelocidad = goku->getCaparazon().getFactorVelocidad();
            goku->setX(goku->x() - velocidad * factorVelocidad);
        }
        break;

    case Qt::Key_D:
        if (!goku->estaSaltando()) {
            float velocidad = goku->getVelocidad();
            float factorVelocidad = goku->getCaparazon().getFactorVelocidad();
            goku->setX(goku->x() + velocidad * factorVelocidad);
        }
        break;

    case Qt::Key_W:
        goku->aumentarFuerzaTiro();
        goku->mostrarTrayectoria(true);
        break;

    case Qt::Key_S:
        goku->disminuirFuerzaTiro();
        goku->mostrarTrayectoria(true);
        break;

    case Qt::Key_Q:
        goku->disminuirAnguloTiro();
        goku->mostrarTrayectoria(true);
        break;

    case Qt::Key_E:
        goku->aumentarAnguloTiro();
        goku->mostrarTrayectoria(true);
        break;

    case Qt::Key_C:
        // Toggle de trayectoria
        goku->toggleTrayectoria(true);
        break;

    case Qt::Key_Z:
        AudioManager::instancia()->reproducirSonido(AudioManager::DISPARO);
        goku->iniciarAnimacionLanzamiento();
        goku->disparar(false);
        break;

    case Qt::Key_X:
        AudioManager::instancia()->reproducirSonido(AudioManager::DISPARO);
        goku->iniciarAnimacionLanzamiento();
        goku->disparar(true);
        break;

    case Qt::Key_Space:
        if (!goku->estaSaltando() && !goku->estaNadando()) {
            AudioManager::instancia()->reproducirSonido(AudioManager::SALTO);
            goku->saltar();
        }
        break;
    }
}

void Juego::procesarTeclasNivelTres(int key, Goku* goku) {
    switch (key) {
    case Qt::Key_A:
        if (goku->estaNadando()) {
            AudioManager::instancia()->reproducirSonido(AudioManager::NADO, 0.5f);
            goku->nadar(-5.0f, 0.0f);
        } else if (!goku->estaSaltando()) {
            float velocidad = goku->getVelocidad();
            float factorVelocidad = goku->getCaparazon().getFactorVelocidad();
            goku->setX(goku->x() - velocidad * factorVelocidad);
        }
        break;

    case Qt::Key_D:
        if (goku->estaNadando()) {
            AudioManager::instancia()->reproducirSonido(AudioManager::NADO, 0.5f);
            goku->nadar(5.0f, 0.0f);
        } else if (!goku->estaSaltando()) {
            float velocidad = goku->getVelocidad();
            float factorVelocidad = goku->getCaparazon().getFactorVelocidad();
            goku->setX(goku->x() + velocidad * factorVelocidad);
        }
        break;

    case Qt::Key_W:
        if (goku->estaNadando()) {
            AudioManager::instancia()->reproducirSonido(AudioManager::NADO, 0.5f);
            goku->nadar(0.0f, -3.0f);
        }
        break;

    case Qt::Key_S:
        if (goku->estaNadando()) {
            AudioManager::instancia()->reproducirSonido(AudioManager::NADO, 0.5f);
            goku->nadar(0.0f, 5.0f);
        }
        break;
    }
}

// ============================================
// ACTUALIZACIÓN DE INTERFAZ
// ============================================

void Juego::actualizarPanelInfo()
{
    if (!nivelActual) return;

    Goku* goku = dynamic_cast<Goku*>(nivelActual->getPersonaje());
    if (!goku) return;

    int numeroNivel = 1;
    switch (nivelActual->getTipo()) {
    case TipoNivel::UNO: numeroNivel = 1; break;
    case TipoNivel::DOS: numeroNivel = 2; break;
    case TipoNivel::TRES: numeroNivel = 3; break;
    }

    int objetivosEntregados = 0;
    if (nivelActual->getTipo() == TipoNivel::DOS) {
        Nivel2* nivel2 = dynamic_cast<Nivel2*>(nivelActual);
        if (nivel2)
            objetivosEntregados = nivel2->getCantidadObjetivosEntregados(); // este método debe existir en Nivel2
    }

    panelInfo->actualizarInformacion(goku, numeroNivel, modoHistoria, objetivosEntregados);
}

// ============================================
// TRANSICIONES Y VENTANAS EMERGENTES
// ============================================

void Juego::mostrarVentanaEmergente(VentanaEmergente::TipoMensaje tipo, int siguienteNivel) {
    // Reproducir sonido según el tipo de ventana
    switch (tipo) {
    case VentanaEmergente::NIVEL_COMPLETADO:
        AudioManager::instancia()->reproducirSonido(AudioManager::NIVEL_COMPLETADO);
        break;
    case VentanaEmergente::GAME_OVER:
        AudioManager::instancia()->reproducirSonido(AudioManager::GAME_OVER);
        break;
    case VentanaEmergente::JUEGO_COMPLETADO:
        AudioManager::instancia()->reproducirSonido(AudioManager::JUEGO_TERMINADO);
        break;
    }

    VentanaEmergente *ventana = new VentanaEmergente(tipo, this);

    connect(ventana, &VentanaEmergente::volverAlMenu, this, &Juego::volverAlMenu);
    connect(ventana, &VentanaEmergente::reintentarNivel, [=]() {
        AudioManager::instancia()->reproducirSonido(AudioManager::CLICK_MENU);
        setGameOver(false);
        cargarNivel(nivelActual->getTipo());
    });
    connect(ventana, &VentanaEmergente::siguienteNivel, [=]() {
        AudioManager::instancia()->reproducirSonido(AudioManager::CLICK_MENU);
        cargarNivel(static_cast<TipoNivel>(siguienteNivel));
    });

    if (!modoHistoria && tipo == VentanaEmergente::NIVEL_COMPLETADO) {
        // No mostrar botón de siguiente nivel
        ventana->forzarModoReintentar();  // Nuevo método que deberás agregar
    }
    ventana->exec();

}

void Juego::mostrarTransicionNivel() {
    if (!nivelActual) return;

    if (!modoHistoria) {
        mostrarVentanaEmergente(VentanaEmergente::NIVEL_COMPLETADO);
        return;
    }

    Goku* goku = dynamic_cast<Goku*>(nivelActual->getPersonaje());
    if (goku) {
        int recolectadas = goku->getEsferasRecolectadas();
        int nuevasEsferas = recolectadas - esferasHistoricasNivelActual;

        if (nuevasEsferas > 0) {
            esferasAcumuladas += nuevasEsferas;
            qDebug() << "[Juego] Esferas nuevas sumadas:" << nuevasEsferas
                     << "-> Total acumuladas:" << esferasAcumuladas;
        } else {
            qDebug() << "[Juego] No se sumaron esferas nuevas. Recolectadas:" << recolectadas
                     << ", ya históricas:" << esferasHistoricasNivelActual;
        }

        esferasHistoricasNivelActual = recolectadas;

        tirosAcumulados = goku->getTirosDisponibles();
        if (goku->tieneCajaLecheObligatoria())
            cajaLecheObligatoriaObtenida = true;
    }

    // Avance de nivel
    switch (nivelActual->getTipo()) {
    case TipoNivel::UNO:
        mostrarVentanaEmergente(VentanaEmergente::NIVEL_COMPLETADO, static_cast<int>(TipoNivel::DOS));
        break;
    case TipoNivel::DOS:
        mostrarVentanaEmergente(VentanaEmergente::NIVEL_COMPLETADO, static_cast<int>(TipoNivel::TRES));
        break;
    case TipoNivel::TRES:
        mostrarVentanaEmergente(VentanaEmergente::JUEGO_COMPLETADO);
        break;
    }
}

// ============================================
// GESTIÓN DE GAME OVER
// ============================================

void Juego::mostrarGameOver() {
    qDebug() << "[Juego] Se activó Game Over.";
    setGameOver(true); // Establecer estado de Game Over

    mostrarVentanaEmergente(VentanaEmergente::GAME_OVER);
}

void Juego::verificarGameOver() {
    if (!nivelActual) return;

    Goku* goku = dynamic_cast<Goku*>(nivelActual->getPersonaje());
    if (!goku) return;

    if (goku->getVidas() <= 0 || panelInfo->getTiempoRestante() <= 0) {
        panelInfo->pausarTiempo();
        timerActualizacionPanel->stop();
        mostrarGameOver();
    }
}

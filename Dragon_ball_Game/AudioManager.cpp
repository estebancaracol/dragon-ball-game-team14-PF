#include "AudioManager.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QTimer>
#include <QResource>
#include <QFileInfo>

AudioManager* AudioManager::m_instancia = nullptr;

AudioManager::AudioManager(QObject *parent)
    : QObject(parent),
    reproductorMusica(nullptr),
    salidaAudioMusica(nullptr),
    volumenMusica(0.5f),  // Volumen más bajo por defecto
    volumenEfectos(1.0f),
    m_musicaActiva(true),
    m_efectosActivos(true)
{
    cargarRutasAudio();
    inicializarAudio();
}

AudioManager::~AudioManager()
{
    if (reproductorMusica) {
        reproductorMusica->stop();
        delete reproductorMusica;
    }

    if (salidaAudioMusica) {
        delete salidaAudioMusica;
    }

    // Limpiar reproductores de efectos
    for (auto reproductor : reproductoresEfectos.values()) {
        if (reproductor) {
            reproductor->stop();
            delete reproductor;
        }
    }

    for (auto salida : salidasAudioEfectos.values()) {
        if (salida) {
            delete salida;
        }
    }
}

AudioManager* AudioManager::instancia()
{
    if (!m_instancia) {
        m_instancia = new AudioManager();
    }
    return m_instancia;
}

void AudioManager::cargarRutasAudio()
{
    // Efectos de recolección
    rutasAudio[ESFERA_DRAGON] = ":/Recursos/Audio/esfera_dragon.wav";
    rutasAudio[CAJA_LECHE] = ":/Recursos/Audio/caja_leche.wav";
    rutasAudio[BURBUJA] = ":/Recursos/Audio/burbuja.wav";

    // Efectos de acción
    rutasAudio[SALTO] = ":/Recursos/Audio/salto.wav";
    rutasAudio[DISPARO] = ":/Recursos/Audio/disparo.wav";
    rutasAudio[NADO] = ":/Recursos/Audio/nado.wav";

    // Efectos de interfaz
    rutasAudio[NIVEL_COMPLETADO] = ":/Recursos/Audio/nivel_completado.wav";
    rutasAudio[GAME_OVER] = ":/Recursos/Audio/game_over.wav";
    rutasAudio[JUEGO_TERMINADO] = ":/Recursos/Audio/juego_terminado.wav";
    rutasAudio[TRANSICION_NIVEL] = ":/Recursos/Audio/transicion.wav";

    // Efectos de menú
    rutasAudio[CLICK_MENU] = ":/Recursos/Audio/click_menu.wav";
    rutasAudio[HOVER_MENU] = ":/Recursos/Audio/hover_menu.wav";

    // Verificar que los recursos existen
    verificarRecursos();

    qDebug() << "[AudioManager] Rutas de audio cargadas";
}

void AudioManager::verificarRecursos()
{
    qDebug() << "[AudioManager] Verificando recursos de audio...";

    for (auto it = rutasAudio.begin(); it != rutasAudio.end(); ++it) {
        TipoSonido tipo = it.key();
        QString ruta = it.value();

        // Verificar si el recurso existe
        QResource recurso(ruta);
        if (recurso.isValid()) {
            qDebug() << "[AudioManager] Recurso encontrado - Tipo:" << tipo << "Ruta:" << ruta;
        } else {
            qDebug() << "[AudioManager] Recurso NO encontrado - Tipo:" << tipo << "Ruta:" << ruta;
        }
    }
}

void AudioManager::inicializarAudio()
{
    // Configurar reproductor de música
    reproductorMusica = new QMediaPlayer(this);
    salidaAudioMusica = new QAudioOutput(this);
    reproductorMusica->setAudioOutput(salidaAudioMusica);

    // Configurar reproductores de efectos
    QList<TipoSonido> tiposEfectos = {
        ESFERA_DRAGON, CAJA_LECHE, BURBUJA, SALTO, DISPARO, NADO,
        NIVEL_COMPLETADO, GAME_OVER, JUEGO_TERMINADO, TRANSICION_NIVEL,
        CLICK_MENU, HOVER_MENU
    };

    for (TipoSonido tipo : tiposEfectos) {
        configurarReproductor(tipo);
    }

    qDebug() << "[AudioManager] Sistema de audio inicializado";
}

QUrl AudioManager::crearUrlRecurso(const QString &ruta)
{
    QResource recurso(ruta);
    if (!recurso.isValid()) {
        qDebug() << "[AudioManager] ERROR: Recurso no válido:" << ruta;
        return QUrl();
    }

    QUrl url("qrc" + ruta);

    if (url.isEmpty() || url.toString().isEmpty()) {
        qDebug() << "[AudioManager] ERROR: URL vacía para recurso:" << ruta;
        return QUrl();
    }

    qDebug() << "[AudioManager] URL creada:" << url.toString() << "para recurso:" << ruta;
    return url;
}

void AudioManager::pausarMusica()
{
    if (reproductorMusica && reproductorMusica->playbackState() == QMediaPlayer::PlayingState) {
        reproductorMusica->pause();
        qDebug() << "[AudioManager] Música pausada";
    }
}

void AudioManager::detenerMusica()
{
    if (reproductorMusica) {
        reproductorMusica->stop();
        qDebug() << "[AudioManager] Música detenida";
    }
}

void AudioManager::setVolumenMusica(float volumen)
{
    volumenMusica = qBound(0.0f, volumen, 1.0f);
    if (salidaAudioMusica) {
        salidaAudioMusica->setVolume(volumenMusica);
    }
    qDebug() << "[AudioManager] Volumen música:" << volumenMusica;
}

void AudioManager::setVolumenEfectos(float volumen)
{
    volumenEfectos = qBound(0.0f, volumen, 1.0f);

    for (auto salida : salidasAudioEfectos.values()) {
        if (salida) {
            salida->setVolume(volumenEfectos);
        }
    }
    qDebug() << "[AudioManager] Volumen efectos:" << volumenEfectos;
}

bool AudioManager::musicaActiva() const
{
    return m_musicaActiva;
}

void AudioManager::setMusicaActiva(bool activa)
{
    m_musicaActiva = activa;

    if (activa) {
        reproducirMusica();
    } else {
        pausarMusica();
    }
}

bool AudioManager::efectosActivos() const
{
    return m_efectosActivos;
}

void AudioManager::setEfectosActivos(bool activos)
{
    m_efectosActivos = activos;

    if (!activos) {
        // Detener todos los efectos en reproducción
        for (auto reproductor : reproductoresEfectos.values()) {
            if (reproductor && reproductor->playbackState() == QMediaPlayer::PlayingState) {
                reproductor->stop();
            }
        }
    }
}

QMediaPlayer* AudioManager::obtenerReproductorDisponible(TipoSonido tipo)
{
    if (!reproductoresEfectos.contains(tipo)) {
        configurarReproductor(tipo);
    }

    return reproductoresEfectos.value(tipo);
}

void AudioManager::testearAudio()
{
    qDebug() << "[AudioManager] === TESTING AUDIO ===";
    qDebug() << "Efectos activos:" << m_efectosActivos;
    qDebug() << "Música activa:" << m_musicaActiva;
    qDebug() << "Reproductores configurados:" << reproductoresEfectos.size();

    // Verificar recursos
    verificarRecursos();

    // Verificar reproductores
    for (auto it = reproductoresEfectos.begin(); it != reproductoresEfectos.end(); ++it) {
        TipoSonido tipo = it.key();
        QMediaPlayer* reproductor = it.value();
        if (reproductor) {
            qDebug() << "Tipo:" << tipo
                     << "Estado:" << reproductor->playbackState()
                     << "Media:" << reproductor->mediaStatus()
                     << "URL:" << reproductor->source().toString();
        }
    }
}

void AudioManager::reproducirSonido(TipoSonido tipo, float volumen)
{
    qDebug() << "[AudioManager] Intentando reproducir sonido tipo:" << tipo;

    // Obtener o crear reproductor
    if (!reproductoresEfectos.contains(tipo)) {
        qDebug() << "[AudioManager] Configurando reproductor para tipo:" << tipo;
        configurarReproductor(tipo);
    }

    QMediaPlayer* reproductor = reproductoresEfectos.value(tipo);
    if (!reproductor) {
        qDebug() << "[AudioManager] ERROR: No se pudo obtener reproductor para tipo:" << tipo;
        return;
    }

    QAudioOutput* salida = salidasAudioEfectos.value(tipo);
    if (!salida) {
        qDebug() << "[AudioManager] ERROR: No se pudo obtener salida para tipo:" << tipo;
        return;
    }

    // Configurar volumen
    float volumenFinal = volumen * volumenEfectos;
    salida->setVolume(volumenFinal);

    reproductor->setPosition(0);

    qDebug() << "[AudioManager] Estado reproductor:" << reproductor->playbackState()
             << "Estado media:" << reproductor->mediaStatus()
             << "Volumen:" << volumenFinal;

    // Reproducir según el estado del media
    switch (reproductor->mediaStatus()) {
    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::EndOfMedia:
        reproductor->play();
        qDebug() << "[AudioManager]  Reproduciendo sonido tipo:" << tipo << " (inmediato)";
        break;

    case QMediaPlayer::LoadingMedia:
        // Media cargándose, esperar y reproducir
        QTimer::singleShot(50, this, [reproductor, tipo]() {
            if (reproductor->mediaStatus() == QMediaPlayer::LoadedMedia) {
                reproductor->setPosition(0);  // Reiniciar posición
                reproductor->play();
                qDebug() << "[AudioManager] Reproduciendo sonido tipo:" << tipo << " (diferido)";
            }
        });
        break;

    case QMediaPlayer::NoMedia:
    case QMediaPlayer::InvalidMedia: {
        // Recargar media
        qDebug() << "[AudioManager] Recargando media para tipo:" << tipo;
        QUrl url = crearUrlRecurso(rutasAudio[tipo]);
        if (!url.isEmpty()) {
            reproductor->setSource(url);
            QTimer::singleShot(100, this, [reproductor, tipo]() {
                if (reproductor->mediaStatus() == QMediaPlayer::LoadedMedia) {
                    reproductor->setPosition(0);  // Reiniciar posición
                    reproductor->play();
                    qDebug() << "[AudioManager] Reproduciendo sonido tipo:" << tipo << " (recargado)";
                } else {
                    qDebug() << "[AudioManager] ERROR: No se pudo cargar media para tipo:" << tipo;
                }
            });
        }
        break;
    }
    }
}

void AudioManager::configurarReproductor(TipoSonido tipo)
{
    if (reproductoresEfectos.contains(tipo)) {
        return; // Ya está configurado
    }

    // Verificar que la ruta existe
    if (!rutasAudio.contains(tipo)) {
        qDebug() << "[AudioManager] ERROR: No hay ruta para tipo:" << tipo;
        return;
    }

    QMediaPlayer* reproductor = new QMediaPlayer(this);
    QAudioOutput* salida = new QAudioOutput(this);

    if (!reproductor || !salida) {
        qDebug() << "[AudioManager] ERROR: No se pudo crear reproductor o salida para tipo:" << tipo;
        return;
    }

    reproductor->setAudioOutput(salida);

    // Configurar URL del recurso
    QUrl url = crearUrlRecurso(rutasAudio[tipo]);
    if (url.isEmpty()) {
        qDebug() << "[AudioManager] ERROR: No se pudo crear URL para tipo:" << tipo;
        delete reproductor;
        delete salida;
        return;
    }

    reproductor->setSource(url);
    salida->setVolume(volumenEfectos);

    connect(reproductor, &QMediaPlayer::mediaStatusChanged, this,
            [reproductor, tipo](QMediaPlayer::MediaStatus status) {
                if (status == QMediaPlayer::EndOfMedia) {
                    // Reiniciar automáticamente la posición cuando termine
                    reproductor->setPosition(0);
                    qDebug() << "[AudioManager] Auto-reset posición para tipo:" << tipo;
                } else if (status == QMediaPlayer::InvalidMedia) {
                    qDebug() << "[AudioManager] Media inválida para tipo:" << tipo;
                }
            });

    // Conectar señales para debug
    connect(reproductor, &QMediaPlayer::errorOccurred, this,
            [tipo](QMediaPlayer::Error error, const QString &errorString) {
                qDebug() << "[AudioManager] Error en reproductor tipo:" << tipo << "Error:" << error << errorString;
            });

    reproductoresEfectos[tipo] = reproductor;
    salidasAudioEfectos[tipo] = salida;

    qDebug() << "[AudioManager] Reproductor configurado para tipo:" << tipo << "URL:" << url.toString();
}

void AudioManager::cargarMusicaFondo(const QString &rutaArchivo)
{
    rutaMusicaFondo = rutaArchivo;

    if (!reproductorMusica) {
        configurarReproductorMusica();
    }

    QResource recurso(rutaArchivo);
    if (!recurso.isValid()) {
        qDebug() << "[AudioManager] ERROR: Archivo de música no encontrado:" << rutaArchivo;
        return;
    }

    QUrl url = crearUrlRecurso(rutaArchivo);
    if (!url.isEmpty()) {
        reproductorMusica->setSource(url);
        qDebug() << "[AudioManager] Música de fondo cargada:" << rutaArchivo;

        setVolumenMusica(0.5f);  // Valor razonable

        // Conectar una vez para cuando se cargue
        connect(reproductorMusica, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::LoadedMedia && m_musicaActiva) {
                reproductorMusica->play();
                qDebug() << "[AudioManager] Música de fondo iniciada tras carga";
            } else if (status == QMediaPlayer::EndOfMedia && m_musicaActiva) {
                reproductorMusica->setPosition(0);
                reproductorMusica->play();
                qDebug() << "[AudioManager] Música de fondo reiniciada (loop)";
            }
        });
    }
}


void AudioManager::configurarReproductorMusica()
{
    if (reproductorMusica) {
        return; // Ya está configurado
    }

    reproductorMusica = new QMediaPlayer(this);
    salidaAudioMusica = new QAudioOutput(this);
    reproductorMusica->setAudioOutput(salidaAudioMusica);

    // Configurar manejo de errores
    connect(reproductorMusica, &QMediaPlayer::errorOccurred, this,
            [this](QMediaPlayer::Error error, const QString &errorString) {
                qDebug() << "[AudioManager] Error en música de fondo:" << error << errorString;
            });

    qDebug() << "[AudioManager] Reproductor de música configurado";
}

void AudioManager::reanudarMusica()
{
    if (!m_musicaActiva || !reproductorMusica) {
        return;
    }

    if (reproductorMusica->playbackState() == QMediaPlayer::PausedState) {
        reproductorMusica->play();
        qDebug() << "[AudioManager] Música reanudada";
    } else if (reproductorMusica->playbackState() == QMediaPlayer::StoppedState) {
        reproducirMusica();
    }
}

void AudioManager::reproducirMusica(bool loop)
{
    if (!m_musicaActiva || !reproductorMusica) {
        return;
    }

    QMediaPlayer::MediaStatus estado = reproductorMusica->mediaStatus();

    if (estado == QMediaPlayer::LoadedMedia) {
        reproductorMusica->play();
        qDebug() << "[AudioManager] Iniciando música de fondo";
    } else if (estado == QMediaPlayer::NoMedia && !rutaMusicaFondo.isEmpty()) {
        cargarMusicaFondo(rutaMusicaFondo);
    } else if (estado == QMediaPlayer::LoadingMedia) {
        qDebug() << "[AudioManager] Música aún cargándose, se reproducirá cuando esté lista";
        // El play se hará automáticamente al cambiar el estado (ver arriba)
    } else {
        qDebug() << "[AudioManager] Estado inesperado al intentar reproducir música:" << estado;
    }
}

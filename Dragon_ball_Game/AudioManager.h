#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QDebug>
#include <QHash>

class AudioManager : public QObject
{
    Q_OBJECT

public:
    enum TipoSonido {
        // Efectos de recolección
        ESFERA_DRAGON,
        CAJA_LECHE,
        BURBUJA,
        // Efectos de acción
        SALTO,
        DISPARO,
        NADO,
        // Efectos de interfaz
        NIVEL_COMPLETADO,
        GAME_OVER,
        JUEGO_TERMINADO,
        TRANSICION_NIVEL,
        // Efectos de menú
        CLICK_MENU,
        HOVER_MENU
    };

    static AudioManager* instancia();
    void inicializarAudio();
    void reproducirSonido(TipoSonido tipo, float volumen = 1.0f);

    // Funciones para música de fondo
    void cargarMusicaFondo(const QString &rutaArchivo);
    void reproducirMusica(bool loop = true);
    void pausarMusica();
    void detenerMusica();
    void reanudarMusica();

    // Configuración de volumen
    void setVolumenMusica(float volumen);
    void setVolumenEfectos(float volumen);

    // Estados
    bool musicaActiva() const;
    void setMusicaActiva(bool activa);
    bool efectosActivos() const;
    void setEfectosActivos(bool activos);

    // Funciones de debug
    void testearAudio();

private:
    explicit AudioManager(QObject *parent = nullptr);
    ~AudioManager();
    static AudioManager* m_instancia;

    // Reproductor de música de fondo
    QMediaPlayer* reproductorMusica;
    QAudioOutput* salidaAudioMusica;

    // Reproductores para efectos
    QHash<TipoSonido, QMediaPlayer*> reproductoresEfectos;
    QHash<TipoSonido, QAudioOutput*> salidasAudioEfectos;

    // Configuración
    float volumenMusica;
    float volumenEfectos;
    bool m_musicaActiva;
    bool m_efectosActivos;

    // Rutas de archivos de audio
    QHash<TipoSonido, QString> rutasAudio;
    QString rutaMusicaFondo;

    // Métodos privados
    void cargarRutasAudio();
    void configurarReproductor(TipoSonido tipo);
    void verificarRecursos();
    QUrl crearUrlRecurso(const QString &ruta);
    QMediaPlayer* obtenerReproductorDisponible(TipoSonido tipo);
    void configurarReproductorMusica();
};

#endif // AUDIOMANAGER_H

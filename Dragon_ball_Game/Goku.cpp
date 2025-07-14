#include "Goku.h"
#include "EsferaDragon.h"
#include "CajaLeche.h"
#include "Burbuja.h"
#include "ObjetoFisico.h"
#include "Fisica.h"
#include "AudioManager.h"

#include <QDebug>
#include <QBrush>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <Qt>
#include <QPen>

// ============================================
// CONSTRUCTOR Y CONFIGURACIÓN INICIAL
// ============================================

Goku::Goku(QObject *parent)
    : Personaje(parent), tirosDisponibles(0), enMovimiento(false),
    teclasPresionadas(), estadoActual(Quieto), estadoAnterior(Quieto),
    indiceAnimacionActual(0)
{
    cargarImagen();
    cargarSprites();
    caparazon.actualizarPeso(1);

    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &Goku::actualizarAnimacion);
    timerAnimacion->start(120);

    //     // Timer para control de movimiento
    //     timerMovimiento = new QTimer(this);
    //     connect(timerMovimiento, &QTimer::timeout, this, &Goku::procesarMovimiento);
    //     timerMovimiento->start(16); // ~60 FPS para movimiento fluido

    timerSpriteDefault = new QTimer(this);
    timerSpriteDefault->setSingleShot(true);
    connect(timerSpriteDefault, &QTimer::timeout, this, &Goku::volverASpriteDefault);

    qDebug() << "[Goku] Constructor completado. Estado inicial:" << estadoActual;
}

void Goku::cargarImagen()
{
    spriteDefault = QPixmap(":/Recursos/personaje/goku.png")
    .scaled(60, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setPixmap(spriteDefault);
}

void Goku::cargarSprites() {
    QStringList rutasLanzamiento = {
        ":/Recursos/gokulanzamiento1.png",
        ":/Recursos/gokulanzamiento2.png",
        ":/Recursos/gokulanzamiento3.png"
    };

    QStringList rutasNadandoDerecha = {
        ":/Recursos/gokunadando1.png",
        ":/Recursos/gokunadando2.png",
        ":/Recursos/gokunadando3.png"
    };

    QStringList rutasNadandoIzquierda = {
        ":/Recursos/gokunadando4.png",
        ":/Recursos/gokunadando5.png",
        ":/Recursos/gokunadando6.png"
    };

    QStringList rutasCaminandoIzquierda = {
        ":/Recursos/goku9.png",
        ":/Recursos/goku10.png",
        ":/Recursos/goku11.png",
        ":/Recursos/goku12.png",
        ":/Recursos/goku13.png"
    };

    QStringList rutasCaminandoDerecha = {
        ":/Recursos/goku1.png",
        ":/Recursos/goku2.png",
        ":/Recursos/goku3.png",
        ":/Recursos/goku4.png",
        ":/Recursos/goku5.png"
    };

    // Cargar y escalar sprites
    for (const QString &ruta : rutasLanzamiento)
        spritesLanzando.append(QPixmap(ruta).scaled(60, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    for (const QString &ruta : rutasNadandoDerecha)
        spritesNadandoDerecha.append(QPixmap(ruta).scaled(60, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    for (const QString &ruta : rutasNadandoIzquierda)
        spritesNadandoIzquierda.append(QPixmap(ruta).scaled(60, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    for (const QString &ruta : rutasCaminandoIzquierda)
        spritesCaminandoIzquierda.append(QPixmap(ruta).scaled(45, 65, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    for (const QString &ruta : rutasCaminandoDerecha)
        spritesCaminandoDerecha.append(QPixmap(ruta).scaled(45, 65, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

// ============================================
// GESTIÓN DE ESTADO Y PROCESAMIENTO
// ============================================

void Goku::procesarEstado() {
    qDebug() << "[Estado] Procesando. Teclas activas:" << teclasPresionadas.size();

    // Si no hay teclas presionadas
    if (teclasPresionadas.isEmpty()) {
        if (enMovimiento) {
            qDebug() << "[Estado] Sin teclas - Deteniendo movimiento";
            enMovimiento = false;
            cambiarEstado(Quieto);
        }
        return;
    }

    // Si está en animación especial, no cambiar estado
    if (estadoActual == Lanzando) {
        qDebug() << "[Estado] En lanzamiento - Ignorando teclas";
        return;
    }

    // Determinar nuevo estado
    EstadoAnimacion nuevoEstado = determinarEstado();
    qDebug() << "[Estado] Nuevo estado determinado:" << nuevoEstado;

    if (nuevoEstado != Quieto) {
        enMovimiento = true;
        cambiarEstado(nuevoEstado);
    }
}

void Goku::cambiarEstado(EstadoAnimacion nuevoEstado) {
    if (estadoActual == nuevoEstado) {
        qDebug() << "[Estado] Estado no cambió:" << nuevoEstado;
        return;
    }

    qDebug() << "[Estado] CAMBIO:" << estadoActual << "=>" << nuevoEstado;

    estadoAnterior = estadoActual;
    estadoActual = nuevoEstado;
    indiceAnimacionActual = 0; // Reiniciar animación

    timerSpriteDefault->stop();

    if (nuevoEstado == Quieto) {
        qDebug() << "[Estado] Programando vuelta a sprite default";
        timerSpriteDefault->start(200);
    }
}

EstadoAnimacion Goku::determinarEstado() {
    // Prioridad: A y D (horizontal)
    if (teclasPresionadas.contains(Qt::Key_A)) {
        return enModoNado ? NadandoIzquierda : CaminandoIzquierda;
    }
    else if (teclasPresionadas.contains(Qt::Key_D)) {
        return enModoNado ? NadandoDerecha : CaminandoDerecha;
    }
    // Solo en agua: W y S (vertical)
    else if (enModoNado && (teclasPresionadas.contains(Qt::Key_W) || teclasPresionadas.contains(Qt::Key_S))) {
        return ultimaDireccion == -1 ? NadandoIzquierda : NadandoDerecha;
    }

    return Quieto;
}

// ============================================
// GESTIÓN DE TECLAS
// ============================================

void Goku::agregarTeclaPresionada(int key) {
    if (!teclasPresionadas.contains(key)) {
        teclasPresionadas.insert(key);
        qDebug() << "[Teclas] Agregada:" << key << "Set actual:" << teclasPresionadas;
        procesarEstado();
    } else {
        qDebug() << "[Teclas] Tecla ya estaba presionada:" << key;
    }
}

void Goku::removerTeclaPresionada(int key) {
    if (teclasPresionadas.contains(key)) {
        teclasPresionadas.remove(key);
        qDebug() << "[Teclas] Removida:" << key << "Set actual:" << teclasPresionadas;
        procesarEstado();
    } else {
        qDebug() << "[Teclas] Tecla no estaba en el set:" << key;
    }
}

// ============================================
// SISTEMA DE ANIMACIÓN
// ============================================

void Goku::actualizarAnimacion() {
    qDebug() << "[Animación] Estado:" << estadoActual << "Frame:" << indiceAnimacionActual
             << "EnMovimiento:" << enMovimiento;

    // Si está quieto, no animar
    if (estadoActual == Quieto) {
        qDebug() << "[Animación] Estado Quieto - No animar";
        return;
    }

    const QVector<QPixmap>* sprites = obtenerSpritesDelEstado(estadoActual);

    if (!sprites || sprites->isEmpty()) {
        qDebug() << "[Animación] ERROR: No hay sprites para estado:" << estadoActual;
        return;
    }

    // Aplicar sprite actual
    int frameIndex = indiceAnimacionActual % sprites->size();
    setPixmap(sprites->at(frameIndex));
    qDebug() << "[Animación] Aplicando frame:" << frameIndex << "de" << sprites->size();

    // Avanzar frame
    indiceAnimacionActual++;

    // Reiniciar si completó el ciclo (excepto para Lanzando)
    if (estadoActual != Lanzando && indiceAnimacionActual >= sprites->size()) {
        indiceAnimacionActual = 0;
        qDebug() << "[Animación] Reiniciando ciclo de animación";
    }

    // Manejo especial para lanzamiento
    if (estadoActual == Lanzando && indiceAnimacionActual >= sprites->size()) {
        qDebug() << "[Animación] Finalizando animación de lanzamiento";
        finalizarAnimacionLanzamiento();
    }
}

void Goku::actualizarSpriteMovimiento(EstadoAnimacion estado) {
    qDebug() << "[Animación] actualizarSpriteMovimiento. Estado nuevo:" << estado << ", Estado actual:" << estadoActual;

    if (estadoAnterior != estado) {
        indiceAnimacionActual = 0;
    }

    estadoAnterior = estadoActual;
    estadoActual = estado;

    QVector<QPixmap>* sprites = nullptr;
    switch (estado) {
    case CaminandoDerecha: sprites = &spritesCaminandoDerecha; break;
    case CaminandoIzquierda: sprites = &spritesCaminandoIzquierda; break;
    case NadandoDerecha: sprites = &spritesNadandoDerecha; break;
    case NadandoIzquierda: sprites = &spritesNadandoIzquierda; break;
    case Lanzando: sprites = &spritesLanzando; break;
    case Quieto:
    default:
        qDebug() << "[Animación] Estado Quieto. No se cambia sprite.";
        return;
    }

    if (sprites && !sprites->isEmpty()) {
        setPixmap(sprites->at(indiceAnimacionActual % sprites->size()));
        qDebug() << "[Animación] Mostrando frame:" << indiceAnimacionActual;
        indiceAnimacionActual++;

        if (estadoActual == Lanzando && indiceAnimacionActual >= sprites->size()) {
            finalizarAnimacionLanzamiento();
        }
    }
}

void Goku::volverASpriteDefault() {
    qDebug() << "[Sprite] Volviendo al sprite por defecto";
    setPixmap(spriteDefault);
    indiceAnimacionActual = 0;
}

const QVector<QPixmap>* Goku::obtenerSpritesDelEstado(EstadoAnimacion estado) {
    switch (estado) {
    case CaminandoDerecha: return &spritesCaminandoDerecha;
    case CaminandoIzquierda: return &spritesCaminandoIzquierda;
    case NadandoDerecha: return &spritesNadandoDerecha;
    case NadandoIzquierda: return &spritesNadandoIzquierda;
    case Lanzando: return &spritesLanzando;
    default:
        qDebug() << "[Sprites] Estado no reconocido:" << estado;
        return nullptr;
    }
}

// ============================================
// SISTEMA DE LANZAMIENTO Y TIRO
// ============================================

void Goku::iniciarAnimacionLanzamiento() {
    qDebug() << "[Lanzamiento] Iniciando animación";
    cambiarEstado(Lanzando);
}

void Goku::finalizarAnimacionLanzamiento() {
    qDebug() << "[Lanzamiento] Finalizando animación";
    cambiarEstado(Quieto);
}

void Goku::aumentarFuerzaTiro() {
    fuerzaTiro = qMin(fuerzaTiro + 1.0f, 40.0f);
    if (trayectoriaActiva) mostrarTrayectoria(true);
    qDebug() << "[Tiro] Fuerza aumentada:" << fuerzaTiro;
}

void Goku::disminuirFuerzaTiro() {
    fuerzaTiro = qMax(fuerzaTiro - 1.0f, 5.0f);
    if (trayectoriaActiva) mostrarTrayectoria(true);
    qDebug() << "[Tiro] Fuerza disminuida:" << fuerzaTiro;
}

void Goku::aumentarAnguloTiro() {
    anguloTiro = qMin(anguloTiro + 5.0f, 85.0f);
    if (trayectoriaActiva) mostrarTrayectoria(true);
    qDebug() << "[Tiro] Ángulo aumentado:" << anguloTiro;
}

void Goku::disminuirAnguloTiro() {
    anguloTiro = qMax(anguloTiro - 5.0f, 5.0f);
    if (trayectoriaActiva) mostrarTrayectoria(true);
    qDebug() << "[Tiro] Ángulo disminuido:" << anguloTiro;
}

void Goku::disparar(bool parabolico)
{
    if (tirosDisponibles <= 0) {
        qDebug() << "[Goku] Sin tiros.";
        return;
    }

    // Reproducir sonido de disparo
    AudioManager::instancia()->reproducirSonido(AudioManager::DISPARO);

    // Crear objeto físico como proyectil
    ObjetoFisico* proyectil = new ObjetoFisico(true);  // 'true' para que se configure como proyectil
    QPointF inicio = mapToScene(boundingRect().center());
    proyectil->setPos(inicio);
    scene()->addItem(proyectil);

    // Calcular velocidad inicial con ayuda de Fisica
    QPointF velocidad = parabolico
                            ? fisica.calcularTiroParabolico(anguloTiro, fuerzaTiro)
                            : fisica.calcularTiroRecto(fuerzaTiro);

    // Lanzar el proyectil desde Fisica
    fisica.lanzarProyectil(proyectil, velocidad, parabolico);

    tirosDisponibles--;
    qDebug() << "[Goku] Disparo realizado. Tiros restantes:" << tirosDisponibles;
    limpiarTrayectoria();
}

void Goku::aumentarTiros(int cantidad) {
    tirosDisponibles += cantidad;
    qDebug() << "[Goku] Tiros aumentados en" << cantidad << ". Total:" << tirosDisponibles;
}

// ============================================
// SISTEMA DE TRAYECTORIA
// ============================================

void Goku::mostrarTrayectoria(bool parabolico)
{
    limpiarTrayectoria();
    if (!scene()) return;

    QPointF origen = mapToScene(boundingRect().center());
    float rad = qDegreesToRadians(parabolico ? anguloTiro : 0.0f);
    float vx = fuerzaTiro * qCos(rad);
    float vy = -fuerzaTiro * qSin(rad);
    float gravedad = 0.5f;
    float dt = 0.1f;
    float t = 0.0f;
    QRectF limites = scene()->sceneRect();

    // Solo crear un punto cada "saltos" de tiempo visual
    const int intervaloVisual = 10;  // Cada cuántos pasos dibujar el punto

    for (int i = 0; i < 500; ++i) {
        float x = origen.x() + vx * t;
        float y = origen.y() + vy * t + (parabolico ? 0.5f * gravedad * t * t : 0.0f);
        QPointF punto(x, y);

        if (!limites.contains(punto)) break;

        if (i % intervaloVisual == 0) {
            auto* elipse = new QGraphicsEllipseItem(-3, -3, 6, 6);
            elipse->setPos(punto);
            elipse->setBrush(QColor(255, 165, 0, 180));
            elipse->setPen(Qt::NoPen);
            elipse->setZValue(10);
            trayectoriaPreview.append(elipse);
            scene()->addItem(elipse);
        }

        t += dt;
    }

    trayectoriaVisible = true;  // Marcar como visible
}

void Goku::limpiarTrayectoria()
{
    for (auto* punto : trayectoriaPreview) {
        if (scene()) {
            scene()->removeItem(punto);
        }
        delete punto;  // Importante: liberar memoria
    }
    trayectoriaPreview.clear();
    trayectoriaVisible = false;  // Marcar como no visible
}

void Goku::actualizarTrayectoria(bool parabolico)
{
    if (trayectoriaVisible) {
        limpiarTrayectoria();
        mostrarTrayectoria(parabolico);
    }
}

void Goku::toggleTrayectoria(bool parabolico)
{
    if (trayectoriaVisible) {
        limpiarTrayectoria();
    } else {
        mostrarTrayectoria(parabolico);
    }
}

// ============================================
// SISTEMA DE NADO
// ============================================

void Goku::activarNado() {
    if (!enModoNado) {
        enModoNado = true;
        setEnSalto(false);
        setVelocidadSalto(0);

        // Reproducir sonido de entrada al agua
        AudioManager::instancia()->reproducirSonido(AudioManager::NADO, 0.7f);

        // Ajustar estado actual si es necesario
        if (estadoActual == CaminandoDerecha) {
            cambiarEstado(NadandoDerecha);
        } else if (estadoActual == CaminandoIzquierda) {
            cambiarEstado(NadandoIzquierda);
        }

        qDebug() << "[Nado] Activado";
    }
}

void Goku::desactivarNado() {
    if (enModoNado) {
        enModoNado = false;

        // Ajustar estado actual si es necesario
        if (estadoActual == NadandoDerecha) {
            cambiarEstado(CaminandoDerecha);
        } else if (estadoActual == NadandoIzquierda) {
            cambiarEstado(CaminandoIzquierda);
        }

        qDebug() << "[Nado] Desactivado";
    }
}

void Goku::nadar(float dx, float dy) {
    if (!enModoNado) return;

    // Reproducir sonido de nado solo si hay movimiento significativo
    if (abs(dx) > 0.1f || abs(dy) > 0.1f) {
        // Reproducir sonido de nado con volumen bajo para evitar saturación
        static int contadorNado = 0;
        contadorNado++;
        if (contadorNado % 10 == 0) { // Solo cada 10 movimientos
            AudioManager::instancia()->reproducirSonido(AudioManager::NADO, 0.3f);
        }
    }

    // Actualizar dirección si hay movimiento horizontal
    if (dx < 0) {
        ultimaDireccion = -1;
    } else if (dx > 0) {
        ultimaDireccion = 1;
    }

    // Mover posición
    setX(x() + dx);
    setY(y() + dy);

    qDebug() << "[Nado] dx:" << dx << "dy:" << dy << "posY:" << y();
}

// ============================================
// SISTEMA DE SALTO
// ============================================

void Goku::saltar() {
    if (!estaSaltando()) {
        float fuerzaSalto = -10.0f * caparazon.getFactorSalto();
        setVelocidadSalto(fuerzaSalto);
        setEnSalto(true);

        // Reproducir sonido de salto
        AudioManager::instancia()->reproducirSonido(AudioManager::SALTO);
    }
}

// ============================================
// SISTEMA DE RECOLECCIÓN DE ITEMS
// ============================================

void Goku::recolectar(Item* item)
{
    if (item && !item->isRecolectado()) {
        item->efecto();
        item->setRecolectado(true);

        if (auto* esfera = dynamic_cast<EsferaDragon*>(item)) {
            esferasRecolectadas++;
            AudioManager::instancia()->reproducirSonido(AudioManager::ESFERA_DRAGON);
            qDebug() << "[Goku] Esfera del Dragon recolectada. Total:" << esferasRecolectadas;
        }
        else if (CajaLeche* caja = dynamic_cast<CajaLeche*>(item)) {
            AudioManager::instancia()->reproducirSonido(AudioManager::CAJA_LECHE);
            if (caja->property("cantidadTiros").toInt() == 5) {
                cajaLecheObligatoriaRecolectada = true;
                aumentarTiros(5);
                qDebug() << "[Goku] Caja de leche obligatoria (5 tiros) recolectada!";
            } else {
                aumentarTiros(caja->property("cantidadTiros").toInt());
                qDebug() << "[Goku] Caja de leche opcional recolectada.";
            }
        }
        else if (auto* burbuja = dynamic_cast<Burbuja*>(item)) {
            burbujasRecolectadas++;
            AudioManager::instancia()->reproducirSonido(AudioManager::BURBUJA);
        }

        qDebug() << "[Goku] Item recolectado.";
    }
}

// ============================================
// SISTEMA DE INVULNERABILIDAD
// ============================================

// void Goku::activarInvulnerabilidad(int ms)
// {
//     if (invulnerable) return;

//     invulnerable = true;

//     if (!timerInvulnerabilidad)
//         timerInvulnerabilidad = new QTimer(this);

//     int intervalo = 200;
//     int repeticiones = ms / intervalo;
//     auto contador = std::make_shared<int>(0);

//     disconnect(timerInvulnerabilidad);

//     connect(timerInvulnerabilidad, &QTimer::timeout, this, [=]() mutable {
//         qreal nuevaOpacidad = (opacity() == 1.0) ? 0.5 : 1.0;
//         setOpacity(nuevaOpacidad);

//         (*contador)++;
//         if (*contador >= repeticiones) {
//             timerInvulnerabilidad->stop();
//             setOpacity(1.0);
//             invulnerable = false;
//         }
//     });

//     timerInvulnerabilidad->start(intervalo);
// }

void Goku::activarInvulnerabilidad(int ms)
{
    if (invulnerable) return;

    invulnerable = true;
    setOpacity(0.5);

    if (!timerInvulnerabilidad)
        timerInvulnerabilidad = new QTimer(this);

    disconnect(timerInvulnerabilidad, nullptr, nullptr, nullptr);

    connect(timerInvulnerabilidad, &QTimer::timeout, this, [=]() {
        timerInvulnerabilidad->stop();
        setOpacity(1.0);
        invulnerable = false;
    });

    timerInvulnerabilidad->start(ms);
}

// ============================================
// GETTERS Y SETTERS
// ============================================

bool Goku::estaNadando() const {
    return enModoNado;
}

int Goku::getTirosDisponibles() const {
    return tirosDisponibles;
}

float Goku::getVelocidad() const {
    return velocidad;
}

float Goku::getFuerzaTiro() const {
    return fuerzaTiro;
}

float Goku::getAnguloTiro() const {
    return anguloTiro;
}

QList<int> Goku::getTeclasPresionadas() const {
    return teclasPresionadas.values();
}

Caparazon& Goku::getCaparazon() {
    return caparazon;
}

int Goku::getEsferasRecolectadas() const {
    return esferasRecolectadas;
}

int Goku::getBurbujasRecolectadas() const {
    return burbujasRecolectadas;
}

bool Goku::tieneCajaLecheObligatoria() const {
    return cajaLecheObligatoriaRecolectada;
}

bool Goku::trayectoriaEstaVisible() const {
    return trayectoriaActiva;
}

EstadoAnimacion Goku::getEstadoAnimacion() const {
    return estadoActual;
}

void Goku::marcarCajaLecheObligatoria() {
    cajaLecheObligatoriaRecolectada = true;
}

void Goku::setProgreso(int esferas, int tiros, bool tieneCajaLeche) {
    esferasRecolectadas = esferas;
    tirosDisponibles = tiros;
    cajaLecheObligatoriaRecolectada = tieneCajaLeche;

    qDebug() << "[Goku] Progreso restaurado: esferas=" << esferasRecolectadas
             << ", tiros=" << tirosDisponibles
             << ", caja leche=" << cajaLecheObligatoriaRecolectada;
}

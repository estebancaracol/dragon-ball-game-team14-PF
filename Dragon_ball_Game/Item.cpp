#include "Item.h"
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QGraphicsScene>
#include <QTimer>

// ============================================
// CONSTRUCTOR
// ============================================
Item::Item()
    : posX(0), posY(0), recolectado(false)
{
}

// ============================================
// GETTERS
// ============================================
bool Item::isRecolectado() const {
    return recolectado;
}

// ============================================
// SETTERS
// ============================================
void Item::setRecolectado(bool estado) {
    if (estado && !recolectado) {
        aplicarEfectoRecolectado();
    } else if (!estado && recolectado) {
        restaurarAparienciaNormal();
    }
    recolectado = estado;
}

// ============================================
// SISTEMA DE EFECTOS VISUALES
// ============================================
void Item::aplicarEfectoRecolectado() {
    if (spriteOriginal.isNull()) {
        spriteOriginal = pixmap();
    }
    setOpacity(0.5);
    qDebug() << "[Item] Efecto de recolección aplicado (opacidad 0.5)";
}

void Item::restaurarAparienciaNormal() {
    if (!spriteOriginal.isNull()) {
        setPixmap(spriteOriginal);
        qDebug() << "[Item] Apariencia normal restaurada";
    }
}

// ============================================
// SISTEMA DE VIDA ÚTIL Y DESAPARICIÓN
// ============================================
void Item::iniciarDesaparicionConParpadeo(int vidaTotalMs, int parpadeoMs) {
    if (!scene()) return;

    // Parpadeo antes de desaparecer
    QTimer::singleShot(vidaTotalMs - parpadeoMs, this, [=]() {
        auto* efecto = new QGraphicsOpacityEffect(this);
        setGraphicsEffect(efecto);
        QPropertyAnimation* anim = new QPropertyAnimation(efecto, "opacity");
        anim->setDuration(parpadeoMs);
        anim->setLoopCount(3);
        anim->setKeyValueAt(0.0, 1.0);
        anim->setKeyValueAt(0.25, 0.0);
        anim->setKeyValueAt(0.5, 1.0);
        anim->setKeyValueAt(0.75, 0.0);
        anim->setKeyValueAt(1.0, 1.0);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    });

    // Eliminar del escenario
    QTimer::singleShot(vidaTotalMs, this, [=]() {
        if (scene()) {
            scene()->removeItem(this);
            delete this;
            qDebug() << "[Item] Eliminado tras vida útil de" << vidaTotalMs << "ms";
        }
    });
}

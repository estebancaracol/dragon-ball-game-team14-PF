#ifndef ITEM_H
#define ITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPixmap>

// ========== CLASE ITEM ==========
class Item : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    // ========== CONSTRUCTORES Y DESTRUCTOR ==========
    Item();
    virtual ~Item() = default;
    using QGraphicsPixmapItem::QGraphicsPixmapItem;

    // ========== GESTIÓN DE ESTADO ==========
    bool isRecolectado() const;
    void setRecolectado(bool recolectado);

    // ========== EFECTOS VISUALES ==========
    void aplicarEfectoRecolectado();
    void restaurarAparienciaNormal();
    void iniciarDesaparicionConParpadeo(int vidaTotalMs = 8000, int parpadeoMs = 1500);

    // ========== MÉTODOS VIRTUALES ==========
    virtual void efecto() = 0;  // función virtual pura

protected:
    // ========== ATRIBUTOS DE POSICIÓN ==========
    float posX;
    float posY;

    // ========== ATRIBUTOS DE ESTADO ==========
    bool recolectado;

    // ========== ATRIBUTOS GRÁFICOS ==========
    QPixmap spriteOriginal; // Para restaurar la apariencia
};

#endif // ITEM_H

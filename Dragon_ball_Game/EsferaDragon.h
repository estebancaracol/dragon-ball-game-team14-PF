#ifndef ESFERADRAGON_H
#define ESFERADRAGON_H

#include "Item.h"
#include <QObject>


class EsferaDragon : public Item
{
    Q_OBJECT
public:
    explicit EsferaDragon(short puntaje = 100);

    void efecto() override;

private:
    short puntaje;
};

#endif // ESFERADRAGON_H

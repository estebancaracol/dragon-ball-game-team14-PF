#ifndef ESFERADRAGON_H
#define ESFERADRAGON_H

#include "Item.h"
#include <QObject>


class EsferaDragon : public Item
{
    Q_OBJECT
public:
    explicit EsferaDragon(short recolectadas = 1, short TotalRecolectar = 7);

    void efecto() override;

private:
    short recolectadas;
    short TotalRecolectar;
};

#endif // ESFERADRAGON_H

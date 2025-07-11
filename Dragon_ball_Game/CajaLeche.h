#ifndef CAJALECHE_H
#define CAJALECHE_H

#include <QObject>
#include "Item.h"

class CajaLeche : public Item
{
    Q_OBJECT

public:
    explicit CajaLeche(short cantidadTiros = 2);

    void efecto() override;
    short getCantidadTiros() const;

private:
    short cantidadTiros;
};

#endif // CAJALECHE_H

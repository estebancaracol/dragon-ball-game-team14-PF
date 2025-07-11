#ifndef BURBUJA_H
#define BURBUJA_H

#include "Item.h"
#include <QObject>


class Burbuja : public Item
{
    Q_OBJECT
public:
    explicit Burbuja(short vidasExtra = 1);

    void efecto() override;

private:
    short vidasExtra;
};

#endif // BURBUJA_H

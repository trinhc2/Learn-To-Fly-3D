#ifndef COIN_H
#define COIN_H

#include "mathLib3D.h"
#include <vector>
#include "rocket.h"

class Coin {
public:
    Point3D position;
    Coin(float x, float y, float z);
};

class CoinSystem {
public:
    std::vector<Coin> v;
    float rotation; //all coins rotate the same
    CoinSystem();
    void update(Rocket& r);
};

#endif
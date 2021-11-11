#include "mathLib3D.h"
#include <vector>

class Coin {
public:
    Point3D position;
    bool collected;
    Coin(float x, float y, float z);
};

class CoinSystem {
public:
    std::vector<Coin> v;
    float rotation; //all coins rotate the same
    CoinSystem();
    void update();
};
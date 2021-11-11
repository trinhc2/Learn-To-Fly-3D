#include "mathLib3D.h"
#include "coin.h"
#include <iostream>

Coin::Coin(float x, float y, float z) {
    position = Point3D(x, y, z);
    collected = false;
}

CoinSystem::CoinSystem() {
    //Hardcoded sample coins
    v.push_back(Coin(0.4,1,1));
    v.push_back(Coin(0.4,-2, -1));
}

void CoinSystem::update(void) {
    //Continuous rotation
    if (rotation > 360){
        rotation = 0;
    }
    else {
        rotation += 1;
    }
    //If coin is collected then remove it from the game (the vector)
    for (std::size_t i=0; i<v.size(); i++) {
        if (v.at(i).collected) {
            v.erase(v.begin() + i);
        }
    }
}
#include "mathLib3D.h"
#include "coin.h"
#include <iostream>

Coin::Coin(float x, float y, float z) {
    position = Point3D(x, y, z);
    collected = false;
}

CoinSystem::CoinSystem() {
  // generate random coins acorss the map
  for(int i=0; i<50; i++) {
	v.push_back(Coin(0.5, (float)rand() / (RAND_MAX) * 1000, (float)rand() / (RAND_MAX) * 10 - 5));
  }
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
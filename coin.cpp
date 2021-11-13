#include "mathLib3D.h"
#include "coin.h"
#include <iostream>
#include "main.h"

Coin::Coin(float x, float y, float z) {
    position = Point3D(x, y, z);
}

CoinSystem::CoinSystem() {
  // generate random coins acorss the map
  for(int i=0; i<50; i++) {
	v.push_back(Coin(0.5, (float)rand() / (RAND_MAX) * 1000, (float)rand() / (RAND_MAX) * 9 - 4.5));
  }
}

void CoinSystem::update(Rocket& r) {
  //Continuous rotation
  if (rotation > 360) {
	rotation = 0;
  } else {
	rotation += 1;
  }
  //If coin is collected then remove it from the game (the vector)
  for (std::size_t i = 0; i < v.size(); i++) {
	  //Obstacles are 1 unit wide (0.5) and rocket is 0.3 units wide (0.15)
    if (inRangeY(v.at(i).position.mY + 0.65, v.at(i).position.mY - 0.65, r)
		  && inRangeZ(v.at(i).position.mZ + 0.65, v.at(i).position.mZ - 0.65, r)) {
	    //if collision: remove coin from game and increment coins total
	    v.erase(v.begin() + i);
	    r.coins += 100;
	  }
  }
}
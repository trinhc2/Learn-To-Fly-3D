#include "mathLib3D.h"
#include "coin.h"
#include "main.h"
#include "objLoader.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <iostream>
#include <string>

Coin::Coin(float x, float y, float z) {
    position = Point3D(x, y, z);
}

CoinSystem::CoinSystem() {
}

void CoinSystem::update(Rocket& r) {
  //Continuous rotation
  if (rotation > 360) {
	rotation = 0;
  } else {
	rotation += 1;
  }
  //If there are less than 5 coins in the system then generate more
  if (v.size() < 5){
    //for the y component, 20 is how far our drawing distance is, so we always spawn it off screen
    //randomFloat(40) arbitrarily chosen to spread them apart
    //randomFloat(9) - 4.5 generates number between -4.5 and 4.5 which is the range our rocket can reach;
    v.push_back(Coin(randomFloat(2.6) - 1.3, randomFloat(40) + r.forwardDistance + 20, randomFloat(9) - 4.5));
  }
  //If coin is collected then remove it from the game (the vector)
  for (std::size_t i = 0; i < v.size(); i++) {
	  //Obstacles are 1 unit wide (0.5) and rocket is 0.3 units wide (0.15)
    if (inRangeX(v.at(i).position.mX + 0.65, v.at(i).position.mX - 0.65, r)
      && inRangeY(v.at(i).position.mY + 0.65, v.at(i).position.mY - 0.65, r)
		  && inRangeZ(v.at(i).position.mZ + 0.65, v.at(i).position.mZ - 0.65, r)) {
	    //if collision: remove coin from game and increment coins total
	    v.erase(v.begin() + i);
	    r.coins += 100;
      coinGet = true; //Tell game coin has been hit
      coinGetAge = 3;//Set timer for text age
	  }
    //If coin is behind rocket, then we don't need to keep track of it anymore
    else if (v.at(i).position.mY + 2< r.position.mY + r.forwardDistance){
      v.erase(v.begin() + i);
    }
  }
}

bool CoinSystem::loadCoinObj(const char* path) {
  bool b = loadOBJ(path, vertexIndices, uvIndices, normalIndices, 
                  out_vertices, out_uvs, out_normals);
  return b;
}

#include "obstacle.h"
#include "mathLib3D.h"
#include "objLoader.h"
#include <stdlib.h>
#include "main.h"

Obstacle::Obstacle() {
  this->position = Point3D();
  this->size = 1;
}

Obstacle::Obstacle(Point3D position, int size) {
  this->position = position;
  this->size = size;
}

ObstacleSystem::ObstacleSystem() {
  // generate obstacles at random across the map
  for (int i = 0; i < 200; i++) {
	v.push_back(Obstacle(Point3D(0.5, randomFloat(1000), randomFloat(10) - 5), 1));
  //randomFloat(10) - 5 generates number between -5 and 5;
  }
}

void ObstacleSystem::update(Rocket& r) {
  //If obstacle is destroyed then remove it from the game (the vector)
  for (std::size_t i = 0; i < v.size(); i++) {
    if (inRangeY(v.at(i).position.mY + 0.65, v.at(i).position.mY - 0.65, r)
		  && inRangeZ(v.at(i).position.mZ + 0.65, v.at(i).position.mZ - 0.65, r)) {
	    //if collision: remove coin from game and increment coins total
	    v.erase(v.begin() + i);
      r.fuel -= r.collisionFuelPenalty;
	  }
    //If obstacle is behind rocket, then we don't need to keep track of it anymore
    else if (v.at(i).position.mY + 2< r.position.mY + r.forwardDistance){
      v.erase(v.begin() + i);
    }    
  }
}

bool ObstacleSystem::loadObstacleObj(const char* path) {
  bool b = loadOBJ(path, vertexIndices, uvIndices, normalIndices, 
                  out_vertices, out_uvs, out_normals);
  return b;
}

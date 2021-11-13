#include "obstacle.h"
#include "mathLib3D.h"

Obstacle::Obstacle() {
  this->position = Point3D();
  this->size = 1;
  this->destroyed = false;
}

Obstacle::Obstacle(Point3D position, int size) {
  this->position = position;
  this->size = size;
  this->destroyed = false;
}

ObstacleSystem::ObstacleSystem() {
  // generate obstacles at random across the map
  for (int i = 0; i < 200; i++) {
	v.push_back(Obstacle(Point3D(0.5, (float)rand() / (RAND_MAX) * 1000, (float)rand() / (RAND_MAX) * 10 - 5), 1));
  }
}

void ObstacleSystem::update(void) {
  //If obstacle is destroyed then remove it from the game (the vector)
  for (std::size_t i = 0; i < v.size(); i++) {
	if (v.at(i).destroyed) {
	  v.erase(v.begin() + i);
	}
  }
}

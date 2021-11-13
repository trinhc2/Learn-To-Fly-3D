#include "obstacle.h"
#include "mathLib3D.h"

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
  for(int i=0; i<200; i++) {
	v.push_back(Obstacle(Point3D(0.5, (float)rand() / (RAND_MAX) * 1000, (float)rand() / (RAND_MAX) * 10 - 5), 1));
  }
}

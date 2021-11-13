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
  v.push_back(Obstacle(Point3D(0.4, 0, 0), 1));
  v.push_back(Obstacle(Point3D(0.8, 0, 0.8), 1));
}

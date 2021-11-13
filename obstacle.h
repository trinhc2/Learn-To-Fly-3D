#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "mathLib3D.h"
#include <vector>
#include "rocket.h"


class Obstacle {
 public:
  Point3D position;
  int size;
  Obstacle();
  Obstacle(Point3D position, int size);
};

class ObstacleSystem {
 public:
  std::vector<Obstacle> v;
  ObstacleSystem();
  void update(Rocket& r);
};

#endif
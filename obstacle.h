#include "mathLib3D.h"
#include <vector>

class Obstacle {
 public:
  Point3D position;
  int size;
  bool destroyed;
  Obstacle();
  Obstacle(Point3D position, int size);
};

class ObstacleSystem {
 public:
  std::vector<Obstacle> v;
  ObstacleSystem();
  void update();
};

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

  std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
  std::vector<Point3D> out_vertices;
  std::vector<Point2D> out_uvs;
  std::vector<Vec3D> out_normals;
  
  bool loadObstacleObj(const char* path);
};

#endif
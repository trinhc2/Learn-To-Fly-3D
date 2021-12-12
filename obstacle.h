#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "mathLib3D.h"
#include <vector>
#include "rocket.h"
#include "particle.h"


class Obstacle {
 public:
  Point3D position;
  int size;
  int type; //denote obstacle types. 0: regular fuel-decreasing obstacles & 1: knock-back bomb obstacles
  Obstacle();
  Obstacle(Point3D position, int size, int type);
  int getSize();
};

class ObstacleSystem {
 public:
  std::vector<Obstacle> v;
  ObstacleSystem();
  void update(Rocket& r, std::vector<Particle>& particles, std::vector<Particle>& rocketflame);

  std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
  std::vector<Point3D> out_vertices;
  std::vector<Point2D> out_uvs;
  std::vector<Vec3D> out_normals;
  
  bool loadObstacleObj(const char* path);
};

#endif
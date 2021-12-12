#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "mathLib3D.h"
#include <vector>
#include "rocket.h"
#include "particle.h"

class SceneObject {
 public:
  Point3D position;
  int size;
  // 0: bird, 1: cloud, 2: stars
  int type;
  SceneObject();
  SceneObject(Point3D position, int size, int type);

  std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
  std::vector<Point3D> out_vertices;
  std::vector<Point2D> out_uvs;
  std::vector<Vec3D> out_normals;
  
  bool loadSceneObj(const char* path);
};

class SceneObjectSystem {
 public:
  std::vector<SceneObject> v;
  SceneObjectSystem();
  void update(Rocket& r, std::vector<Particle>& particles, std::vector<Particle>& rocketflame);
};

#endif
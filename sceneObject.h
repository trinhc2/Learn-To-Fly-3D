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
  int rotation; // This is an int because we only want to rotate about y axis
  // 0: cloud, 1: star
  int type;
  SceneObject();
  SceneObject(Point3D position, int size, int rotation, int type);

  std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
  std::vector<Point3D> out_vertices;
  std::vector<Point2D> out_uvs;
  std::vector<Vec3D> out_normals;
  
  bool loadSceneObj(int type);
};

class SceneObjectSystem {
 public:
  std::vector<SceneObject> v;
  SceneObjectSystem();
  void update(Rocket& r);
};

#endif
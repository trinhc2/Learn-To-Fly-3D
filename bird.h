#ifndef BIRD_H
#define BIRD_H

#include "mathLib3D.h"
#include <vector>

class Bird {
 public:
  Point3D position;
  int animationPhase;
  SceneObject();
  SceneObject(Point3D position, int size, int type);

  std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
  std::vector<Point3D> out_vertices;
  std::vector<Point2D> out_uvs;
  std::vector<Vec3D> out_normals;
  
  bool loadSceneObj(const char* path);
};

#endif
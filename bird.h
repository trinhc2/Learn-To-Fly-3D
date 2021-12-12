#ifndef BIRD_H
#define BIRD_H

#include "mathLib3D.h"
#include "sceneObject.h"
#include <vector>

class Bird{
 public:
  int animationPhase;
  Point3D position;
  Bird();
  Bird(Point3D position, int animationPhase);

  std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
  std::vector<Point3D> out_vertices;
  std::vector<Point2D> out_uvs;
  std::vector<Vec3D> out_normals;
  
  bool loadBirdObj(const char* path);
};

class BirdSystem : public SceneObject {
  public:
    std::vector<Bird> birdPhases;
    BirdSystem();
    BirdSystem(Point3D position, int size, int type);
    void generateBird();
};

#endif
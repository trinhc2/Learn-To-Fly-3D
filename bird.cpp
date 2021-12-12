#include "bird.h"
#include "mathLib3D.h"
#include "objLoader.h"
#include <stdlib.h>
#include "main.h"
#include <iostream>

Bird::Bird() {
  this->position = Point3D();
  loadBirdObj("./assets/Eagle0.obj");
}

Bird::Bird(Point3D position, int animationPhase) {
  this->position = position;
  this->animationPhase = animationPhase;
  std::string objFile = "./assets/Eagle" + std::to_string(animationPhase) + ".obj";
  loadBirdObj(objFile.c_str());
}

bool Bird::loadBirdObj(const char *path) {
  bool b = loadOBJ(path, vertexIndices, uvIndices, normalIndices,
				   out_vertices, out_uvs, out_normals);
  return b;
}

BirdSystem::BirdSystem() {
  this->position = Point3D();
  this->size = 1;
  this->type = 0;
}

BirdSystem::BirdSystem(Point3D position, int size, int type) {
  this->position = Point3D(position);
  this->size = size;
  this->type = type;
}
// For scenic elements, ex. stars, clouds, moon, birds, etc.
// These are *not* obstacles
#include "sceneObject.h"
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
  loadBirdObj("./assets/Eagle" + animationPhase + ".obj");
}

bool Bird::loadBirdObj(const char *path) {
  bool b = loadOBJ(path, vertexIndices, uvIndices, normalIndices,
				   out_vertices, out_uvs, out_normals);
  return b;
}

// For scenic elements, ex. stars, clouds, moon, birds, etc.
// These are *not* obstacles
#include "sceneObject.h"
#include "mathLib3D.h"
#include "objLoader.h"
#include <stdlib.h>
#include "main.h"
#include <iostream>

SceneObject::SceneObject() {
  this->position = Point3D();
  this->size = 1;
  this->type = 0;
}

// TODO: Add a vector containing all bird objects for animation
SceneObject::SceneObject(Point3D position, int size, int type) {
  this->position = position;
  this->size = size;
  this->type = type;
}

bool sceneObject::loadSceneObj(const char *path) {
  bool b = loadOBJ(path, vertexIndices, uvIndices, normalIndices,
				   out_vertices, out_uvs, out_normals);
  return b;
}

SceneObjectSystem::SceneObjectSystem() {
}

void SceneObjectSystem::update() {
  if (v.size() < 10) {
    //for the y component, 20 is how far our drawing distance is, so we always spawn it off screen
    //randomFloat(40) to spread them apart
    //randomFloat(9) - 4.5 generates number between -4.5 and 4.5 which is the range our rocket can reach;
    //generate obstacle of random types from all supported types
    v.push_back(Obstacle(Point3D(randomFloat(2.6) - 1.3, randomFloat(40) + r.forwardDistance + 20, randomFloat(9) - 4.5), 1, rand() % 3));
  }
  for (std::size_t i = 0; i < v.size(); i++) {
    // Bird flies from one side of the screen to the other, so update the position and type of obj (to simulate animation)
    if (v.at(i).type == 0) {
      v.at(i).
    }
    //If obstacle is behind rocket, then we don't need to keep track of it anymore
    if (v.at(i).position.mY + 2 < r.position.mY + r.forwardDistance) {
    v.erase(v.begin() + i);
    }
  }
}

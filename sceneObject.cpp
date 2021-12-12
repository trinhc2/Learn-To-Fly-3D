// For scenic elements, ex. stars, clouds, moon, birds, etc.
// These are *not* obstacles
#include "bird.h"
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
  loadSceneObj(type);
}

SceneObject::SceneObject(Point3D position, int size, int type) {
  this->position = position;
  this->size = size;
  this->type = type;
  loadSceneObj(type);
}

bool SceneObject::loadSceneObj(int type) {
  std::string path;
  switch (type) {
    case 1:
      path = "./assets/scenery/cloud.obj";
      break;
    case 2:
      path = "./assets/scenery/tmp.obj";
      break;
  }
  bool b = loadOBJ(path.c_str(), vertexIndices, uvIndices, normalIndices,
				   out_vertices, out_uvs, out_normals);
  return b;
}

SceneObjectSystem::SceneObjectSystem() {
}

void SceneObjectSystem::update(Rocket& r) {
  if (v.size() < 10) {
    //for the y component, 20 is how far our drawing distance is, so we always spawn it off screen
    //randomFloat(40) to spread them apart
    //randomFloat(9) - 4.5 generates number between -4.5 and 4.5 which is the range our rocket can reach;
    //generate obstacle of random types from all supported types
    int type = rand() % 3;
    if (type == 0) {
      // v.push_back(BirdSystem(Point3D(randomFloat(2.6) - 1.3, randomFloat(40) + r.forwardDistance + 20, randomFloat(9) - 4.5), 1, 0));
    } else {
      v.push_back(SceneObject(Point3D(randomFloat(2.6) - 1.3, randomFloat(40) + r.forwardDistance + 20, randomFloat(9) - 4.5), 1, type));
    }
  }
  for (std::size_t i = 0; i < v.size(); i++) {
    // TODO Figure out the bird stuff
    // Bird flies from one side of the screen to the other, so update the position and type of obj (to simulate animation)
    // if (v.at(i).type == 0) {
    //   if (v.at(i).birdPhases.size() < 8) {
    //     v.at(i).birdPhases.generateBird();
    //   }
    // }
    //If obstacle is behind rocket, then we don't need to keep track of it anymore
    if (v.at(i).position.mY + 2 < r.position.mY + r.forwardDistance) {
      v.erase(v.begin() + i);
    }
  }
}

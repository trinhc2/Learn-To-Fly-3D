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
  this->rotation = 0;
  this->type = 0;
  loadSceneObj(type);
}

SceneObject::SceneObject(Point3D position, int size, int rotation, int type) {
  this->position = position;
  this->size = size;
  this->rotation = rotation;
  this->type = type;
  loadSceneObj(type);
}

bool SceneObject::loadSceneObj(int type) {
  std::string path;
  switch (type) {
    case 0:
      path = "./assets/scenery/cloud.obj";
      break;
    case 1:
      path = "./assets/scenery/star.obj";
      break;
  }
  bool b = loadOBJ(path.c_str(), vertexIndices, uvIndices, normalIndices,
				   out_vertices, out_uvs, out_normals);
  return b;
}

SceneObjectSystem::SceneObjectSystem() {
}

void SceneObjectSystem::update(Rocket& r) {
  if (v.size() < 20) {
    //for the y component, 20 is how far our drawing distance is, so we always spawn it off screen
    //randomFloat(40) to spread them apart
    //randomFloat(15) - 7.5 generates random number within our screen (does not need to be reachable by rocket since it's just visual effects)
    //randomFloat(90) - 45 generates random angle between -45 and 45 to rotate our scenery obj
    //randomFloat(2) + 0.5 generates random size between 0.5 and 2.5
    //generate object of random types from all supported types
    int type = -1;
    float rot = 0;
    float size = 1;
    if (r.forwardDistance >= 20 && r.forwardDistance < 200) {
      type = 0; // cloud
    // Spawn both clouds and stars at an in-between distance
    } else if (r.forwardDistance >= 200 && r.forwardDistance < 300) {
      type = rand() % 2;
    } else if (r.forwardDistance >= 300) {
      type = 1; // star
      rot = randomFloat(90) - 45; // rotate star to make it look nicer
    }
    // Only start generating scenery once we reach a certain height
    if (type > -1) {
      v.push_back(SceneObject(Point3D(randomFloat(2.6) - 1.3, randomFloat(40) + r.forwardDistance + 20, randomFloat(15) - 7.5), 
                  randomFloat(2) - 0.5, rot, type));
    }
  }
  for (std::size_t i = 0; i < v.size(); i++) {
    SceneObject obj = v.at(i);
    // Change rotation so it looks like the star "twinkles"
    if (obj.type == 1) {
      v.at(i).rotation += 1;
    }
    //If object is behind rocket, then we don't need to keep track of it anymore
    if (obj.position.mY + 2 < r.position.mY + r.forwardDistance) {
      v.erase(v.begin() + i);
    }
  }
}
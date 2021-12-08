#include "rocket.h"
#include "coin.h"
#include "mathLib2D.h"
#include "mathLib3D.h"
#include "objLoader.h"
#include <cstdio>
#include <cstring>
#include <vector>
#include <iostream>
#include <string>

Rocket::Rocket() {
  position = Point3D(0.5, -5.45, zOffset);
  angle = 0;
  fuelUpgrades = 0;
  initialFuel = 100;
  fuel = 0;
  coins = 0;
  zOffset = 0;
  forwardDistance = 0;
  forwardSpeed = 0.1;
  turningSpeed = 0.2;
  collisionFuelPenalty = 20;
}

void Rocket::update() {
  //While the rocket has fuel move forward and decrement fuel gauge
  if (fuel > 0) {
	forwardDistance += forwardSpeed;
	fuel -= 0.1;
  } else {
	fuel = 0;
  }
  position.mZ = zOffset;
}

bool Rocket::loadRocketObj(const char* path) {
    bool b = loadOBJ(path, vertexIndices, uvIndices, normalIndices, 
                    out_vertices, out_uvs, out_normals);
    return b;
}
#ifndef ROCKET_H
#define ROCKET_H

#include "mathLib2D.h"
#include "mathLib3D.h"
#include <vector>

class Rocket {
 public:
  Point3D position;
  float angle; //Rocket's angle as a result from turning
  float fuelUpgrades; //Bonus purchased fuel
  float initialFuel;
  float fuel; //Fuel in tank
  float zOffset; //Rockets z position as a result from turning
  float forwardDistance; //how much the rocket has traveled
  float forwardSpeed;
  int collisionFuelPenalty; // how much fuel to decrement as a penalty of colliding with obstacles
  int coins; //coins gathered
  Rocket();
  void update();

  std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
  std::vector<Point3D> out_vertices;
  std::vector<Point2D> out_uvs;
  std::vector<Vec3D> out_normals;

  bool loadRocketObj(const char* path);

};

#endif
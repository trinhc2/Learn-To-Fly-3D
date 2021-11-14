#include "rocket.h"
#include "coin.h"

Rocket::Rocket() {
  position = Point3D(0.5, -5.45, zOffset);
  angle = 0;
  fuelUpgrades = 0;
  initialFuel = 1000;
  fuel = 0;
  coins = 100;
  zOffset = 0;
  forwardDistance = 0;
  forwardSpeed = 0.1;
  collisionFuelPenalty = 20;
}

void Rocket::update() {
  //While the rocket has fuel move forward and decrement fuel gauge
  if (fuel > 0) {
	forwardDistance += forwardSpeed;
	fuel -= forwardSpeed;
  } else {
	fuel = 0;
  }
  position.mZ = zOffset;
}
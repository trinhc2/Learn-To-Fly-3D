#include "obstacle.h"
#include "mathLib3D.h"
#include "objLoader.h"
#include <stdlib.h>
#include "main.h"
#include <iostream>

Obstacle::Obstacle() {
  this->position = Point3D();
  this->size = 1;
  this->type = 0;
}

Obstacle::Obstacle(Point3D position, int size, int type) {
  this->position = position;
  this->size = size;
  this->type = type;
}

ObstacleSystem::ObstacleSystem() {
}

void ObstacleSystem::update(Rocket &r, std::vector<Particle>& particles, std::vector<Particle>& rocketflame) {
  //If there are less than 10 obstacles in the system, then generate more
  if (v.size() < 10) {
    //for the y component, 20 is how far our drawing distance is, so we always spawn it off screen
    //randomFloat(40) to spread them apart
    //randomFloat(9) - 4.5 generates number between -4.5 and 4.5 which is the range our rocket can reach;
    //generate obstacle of random types from all supported types
    v.push_back(Obstacle(Point3D(randomFloat(2.6) - 1.3, randomFloat(40) + r.forwardDistance + 20, randomFloat(9) - 4.5), 1, rand() % 2));
  }
  //If obstacle is destroyed then remove it from the game (the vector)
  for (std::size_t i = 0; i < v.size(); i++) {

    if (inRangeX(v.at(i).position.mX + 0.65, v.at(i).position.mX - 0.65, r)
      && inRangeY(v.at(i).position.mY + 0.65, v.at(i).position.mY - 0.65, r)
      && inRangeZ(v.at(i).position.mZ + 0.65, v.at(i).position.mZ - 0.65, r)) {

      // regular obstacle will deduct rocket's fuel upon collision
      if (v.at(i).type == 0) {
        if (r.fuel > r.collisionFuelPenalty) {
          r.fuel -= r.collisionFuelPenalty;
        } else {
          r.fuel = 0;
        }
        obstacleHit = true; //Tell game obstacle has been hit
        obsHitAge = 3; //Set time for text to display on screen

      } else if(v.at(i).type == 1) {
      // bomb obstacle: knock back the rocket but doesn't deduct fuel
        for (size_t j = 0; j < 50; j++) //add 50 particles to the explosion vector
        {
          Particle p = Particle();
          p.position = v.at(i).position;
          particles.push_back(p);
        }

        rocketflame.clear(); //remove existing rocket flame particles because we send the rocket backwards
        r.forwardDistance -= 10;

      }

      //if collision: remove obstacle from game and decrement fuel total
      v.erase(v.begin() + i);
    }
      //If obstacle is behind rocket, then we don't need to keep track of it anymore
    else if (v.at(i).position.mY + 2 < r.position.mY + r.forwardDistance) {
      v.erase(v.begin() + i);
    }
  }

  //update explosion particle positions
  for(std::size_t i=0; i<particles.size(); i++){
    Vec3D direction = particles.at(i).direction.normalize();

    particles.at(i).position.mX += direction.mX * particles.at(i).speed;
    particles.at(i).position.mY += direction.mY * particles.at(i).speed;
    particles.at(i).position.mZ += direction.mZ * particles.at(i).speed;

    particles.at(i).age -= 0.01;
    if (particles.at(i).age < 0){
        particles.erase(particles.begin() + i);
    }
  }
}

bool ObstacleSystem::loadObstacleObj(const char *path) {
  bool b = loadOBJ(path, vertexIndices, uvIndices, normalIndices,
				   out_vertices, out_uvs, out_normals);
  return b;
}

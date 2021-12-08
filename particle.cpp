#include "particle.h"
#include "main.h"
#include <iostream>

Particle::Particle(Point3D point) {
    //Generate x and z directions
    position = point;
    float x = randomFloat(0.5);
    float z = randomFloat(0.5);
    if (randomFloat(1.0) < 0.5) {
        x = x*-1;
    }
    if (randomFloat(1.0) < 0.5) {
        z = z*-1;
    }
    direction = Vec3D(x, -0.05, z);
    speed = 0.01;
    size = 1;
    r = 1.0;
    g = randomFloat(0.4);
    b = 0;
    age = 2;
}

Particle::Particle() { //constructor for explosion
    direction = Vec3D(randomFloat(2) - 1,randomFloat(2) - 1,randomFloat(2) - 1); //random outwards direction
    speed = 0.05;
    size = 2;
    age = 0.5;
}

ParticleSystem::ParticleSystem() {
    origin = Point3D(0, -5.38, 0.025);
}

void ParticleSystem::update(Rocket& r) {
        for(std::size_t i=0; i<v.size(); i++){
            Vec3D direction = v.at(i).direction.normalize();

            v.at(i).position.mX += direction.mX * v.at(i).speed;
            v.at(i).position.mY += direction.mY * v.at(i).speed;
            v.at(i).position.mZ += direction.mZ * v.at(i).speed;


            v.at(i).age -= 0.01;
            if (v.at(i).age < 0){
                v.erase(v.begin() + i);
            }


        }
    origin.mX = r.position.mX;
    origin.mZ = r.zOffset + 0.025;
	origin.mY = r.forwardDistance - 5.38;
    Particle p = Particle(origin);
    v.push_back(p);
}
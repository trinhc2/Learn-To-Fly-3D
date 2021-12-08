#ifndef PARTICLE_H
#define PARTICLE_H

#include "mathLib3D.h"
#include <vector>
#include "rocket.h"

class Particle {
public:
    Point3D position;
    Vec3D direction;
    float speed;
    int size;
    float r;
    float g;
    float b;
    float age;
    Particle(Point3D point);
    Particle();
};

class ParticleSystem {
public: 
    Point3D origin;
    std::vector<Particle> v;
    ParticleSystem();
    void update(Rocket& r);
};
#endif
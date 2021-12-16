#ifndef COIN_H
#define COIN_H

#include "mathLib2D.h"
#include "mathLib3D.h"
#include <vector>
#include "rocket.h"

class Coin {
public:
    Point3D position;
    Coin(float x, float y, float z);
};

class CoinSystem {
public:
    std::vector<Coin> v;
    float rotation; //all coins rotate the same
    CoinSystem();
    void update(Rocket& r);
    
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<Point3D> out_vertices;
    std::vector<Point2D> out_uvs;
    std::vector<Vec3D> out_normals;

    bool loadCoinObj(const char* path);
};

#endif
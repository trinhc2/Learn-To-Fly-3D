#include "mathLib2D.h"
#include "mathLib3D.h"
#include <vector>

class Rocket {
public:
    Point3D position;
    float angle; //Rocket's angle as a result from turning
    float fuelUpgrades; //Bonus purchased fuel
    float fuel; //Fuel in tank
    float zOffset; //Rockets z position as a result from turning
    float forwardDistance; //how much the rocket has traveled
    int coins; //coins gathered
    Rocket();
    void update();

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<Point3D> temp_vertices;
    std::vector<Point2D> temp_uvs;
    std::vector<Vec3D> temp_normals;
    
    bool loadOBJ(const char* path);

};
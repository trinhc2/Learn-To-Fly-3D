#include "mathLib3D.h"

class Rocket {
public:
    Point3D position;
    float angle; //Rocket's angle as a result from turning
    float fuel; //Fuel in tank
    float zOffset; //Rockets z position as a result from turning
    float forwardDistance; //how much the rocket has traveled
    int coins; //coins gathered
    Rocket();
    void update();
};
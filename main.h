#ifndef MAIN_SHARED_FUCNTION_H
#define MAIN_SHARED_FUNCTION_H

#include "rocket.h"

extern bool coinGet;
extern bool obstacleHit;
extern float obsHitAge;
extern float coinGetAge;

bool inRangeX(float high, float low, Rocket r);

bool inRangeZ(float high, float low, Rocket r);

bool inRangeY(float high, float low, Rocket r);

float randomFloat(float x);
#endif
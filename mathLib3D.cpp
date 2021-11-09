#include "mathLib3D.h"
#include <cmath>

Point3D::Point3D(float inX, float inY, float inZ){
    this->mX = inX;
    this->mY = inY;
    this->mZ = inZ;
}

Point3D::Point3D(){

}

float Point3D::distanceTo(Point3D other){
    float deltaX = pow(other.mX - mX, 2.0);
    float deltaY = pow(other.mY - mY, 2.0);
    float deltaZ = pow(other.mZ - mZ, 2.0);
    return sqrt(deltaX + deltaY + deltaZ);
}

float Point3D::fastDistanceTo(Point3D other){
    float deltaX = pow(other.mX - mX, 2.0);
    float deltaY = pow(other.mY - mY, 2.0);
    float deltaZ = pow(other.mZ - mZ, 2.0);
    return (deltaX + deltaY + deltaZ);
}

Vec3D::Vec3D(float inX, float inY, float inZ){
    mX = inX;
    mY = inY;
    mZ = inZ;
}

Vec3D::Vec3D(){

}

float Vec3D::length() {
    float sum = pow(mX, 2.0) + pow(mY, 2.0) + pow(mZ, 2.0);
    return sqrt(sum);
}

Vec3D Vec3D::normalize() {
    float x = mX / length();
    float y = mY / length();
    float z = mZ / length();
    return Vec3D(x, y, z);
}

Vec3D Vec3D::multiply(float scalar){
    return Vec3D(mX * scalar, mY * scalar, mZ * scalar);
}

Vec3D Vec3D::createVector(Point3D p1, Point3D p2){
    return Vec3D(p2.mX - p1.mX, p2.mY - p1.mY, p2.mZ - p1.mZ);
}

Point3D Vec3D::movePoint(Point3D source){
    return Point3D(source.mX + mX, source.mY +mY, source.mZ +mZ);
}
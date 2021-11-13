#include "mathLib2D.h"
#include <math.h>

Point2D::Point2D() {
  this->mX = 0;
  this->mY = 0;
}

Point2D::Point2D(float inX, float inY) {
  this->mX = inX;
  this->mY = inY;
}

float Point2D::distanceTo(Point2D other) {
  return sqrt(pow((other.mX - this->mX), 2) + pow((other.mY - this->mY), 2));
}
float Point2D::fastDistanceTo(Point2D other) {
  return pow((other.mX - this->mX), 2) + pow((other.mY - this->mY), 2);
}

Vec2D::Vec2D() {
  this->mX = 0;
  this->mY = 0;
}

Vec2D::Vec2D(float inX, float inY) {
  this->mX = inX;
  this->mY = inY;
}

float Vec2D::length() {
  return sqrt(pow(this->mX, 2) + pow(this->mY, 2));
}

Vec2D Vec2D::normalize() {
  float len = this->length();
  return Vec2D(this->mX/len, this->mY/len);
}

Vec2D Vec2D::multiply(float scalar) {
  return Vec2D(this->mX*scalar, this->mY*scalar);
}

Point2D Vec2D::movePoint(Point2D source) {
  return Point2D(source.mX+this->mX, source.mY+this->mY);
}

Vec2D Vec2D::createVector(Point2D p1, Point2D p2) {
  return Vec2D(p2.mX - p1.mX, p2.mY - p1.mY);
}
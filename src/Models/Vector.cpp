//
// Created by Elias Aggergaard Larsen on 11/06/2025.
//

#include "Vector.h"
#include <cmath>

Vector::Vector(int x, int y) : x(x), y(y)
{
}

Vector Vector::operator-(const Vector &other) const
{
  return Vector(x - other.x, y - other.y);
}

Vector Vector::operator+(const Vector &other) const
{
  return Vector(x + other.x, y + other.y);
}

Vector Vector::operator*(double scalar) const
{
  return Vector(x * scalar, y * scalar);
}

double Vector::dot(const Vector &a, const Vector &b) const
{
  return a.x * b.x + a.y * b.y;
}

Vector Vector::closestVectorFromPoint(const Vector& vectorStart , const Vector &fromPoint) const
{
  Vector AP = fromPoint - vectorStart;
  double t = dot(AP, *this) / dot(*this, *this);
  Vector Q = vectorStart + *this * t;        // Closest point on the line
  return Q - fromPoint;                // Vector from point P to closest point
}

bool Vector::isNullVector() const
{
  return x == 0 && y == 0;
}

double Vector::getLength() const
{
  return std::sqrt((x * x) + (y * y));
}

std::string Vector::toString() const
{
  return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

bool Vector::hasSmallerValueThan(Vector v) const
{
  return getSmallestValue() < v.getSmallestValue();
}

int Vector::getSmallestValue() const
{
  return (x > y) ? x : y;
}

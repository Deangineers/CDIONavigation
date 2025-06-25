//
// Created by Elias Aggergaard Larsen on 11/06/2025.
//

#include "Vector.h"

#include <algorithm>
#include <cmath>
#include <numeric>

Vector::Vector(int x, int y) : x(x), y(y)
{
}

Vector Vector::operator-(const Vector& other) const
{
  return Vector(x - other.x, y - other.y);
}

Vector Vector::operator+(const Vector& other) const
{
  return Vector(x + other.x, y + other.y);
}

Vector Vector::operator*(double scalar) const
{
  return Vector(x * scalar, y * scalar);
}

double Vector::dot(const Vector& a, const Vector& b) const
{
  return a.x * b.x + a.y * b.y;
}

Vector Vector::closestVectorFromPoint(const Vector& vectorStart, const Vector& fromPoint) const
{
  Vector AP = fromPoint - vectorStart;
  double denom = dot(*this, *this);
  if (denom == 0) return {0, 0};

  double t = dot(AP, *this) / denom;
  t = std::clamp(t, 0.0, 1.0);

  Vector Q = vectorStart + (*this * t);
  return Q - fromPoint;
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

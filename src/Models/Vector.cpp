//
// Created by Elias Aggergaard Larsen on 11/06/2025.
//

#include "Vector.h"
#include <cmath>

Vector::Vector(int x, int y) : x(x), y(y)
{
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

//
// Created by Elias Aggergaard Larsen on 11/06/2025.
//

#include "Vector.h"
#include <cmath>

Vector::Vector(int x, int y) : x(x), y(y)
{
}

bool Vector::isNullVector()
{
  return x == 0 && y == 0;
}

double Vector::getLength()
{
  return std::sqrt((x * x) + (y * y));
}

std::string Vector::toString()
{
  return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

bool Vector::hasSmallerValueThan(Vector v)
{
  return getSmallestValue() < v.getSmallestValue();
}

int Vector::getSmallestValue()
{
  return (x > y) ? x : y;
}

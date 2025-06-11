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

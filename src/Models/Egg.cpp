//
// Created by Elias Aggergaard Larsen on 15/06/2025.
//

#include "Egg.h"

Egg::Egg(const int x1, const int y1, const int x2, const int y2) : VectorWithStartPos(x1, y1, Vector(x2 - x1, y2 - y1))
{
  auto horizontalVector = Vector(x2 - x1, 0);
  auto verticalVector = Vector(0, y2 - y1);

  vectors_.emplace_back(x1, y1, horizontalVector);
  vectors_.emplace_back(x1, y2, horizontalVector);
  vectors_.emplace_back(x1, y1, verticalVector);
  vectors_.emplace_back(x2, y1, verticalVector);
}

Vector Egg::closestVectorFromPoint(const Vector& fromPoint) const
{
  Vector smallestVector = {INT_MAX,INT_MAX};
  for (const auto& vector : vectors_)
  {
    auto closestVector = vector.closestVectorFromPoint(fromPoint);
    if (closestVector.getLength() < smallestVector.getLength())
    {
      smallestVector = closestVector;
    }
  }
  return smallestVector;
}

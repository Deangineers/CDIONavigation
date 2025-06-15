//
// Created by fbas on 13/06/25.
//

#include "VectorWithStartPos.h"

VectorWithStartPos::VectorWithStartPos(int x, int y, const Vector& vector) : Vector(vector), startX_(x), startY_(y)
{
}

int VectorWithStartPos::startX() const
{
  return startX_;
}

int VectorWithStartPos::startY() const
{
  return startY_;
}

Vector VectorWithStartPos::minimalPoints() const
{
  return {getLowestX(), getLowestY()};
}

Vector VectorWithStartPos::maxPoints() const
{
  return {getMaxX(), getMaxY()};
}

Vector VectorWithStartPos::closestVectorFromPoint(const Vector& fromPoint) const
{
  return Vector::closestVectorFromPoint({startX_, startY_}, fromPoint);
}

int VectorWithStartPos::getLowestX() const
{
  if (x > 0)
  {
    return startX_;
  }
  else
  {
    return startX_ + x;
  }
}

int VectorWithStartPos::getLowestY() const
{
  if (y > 0)
  {
    return startY_;
  }
  else
  {
    return startY_ + y;
  }
}

int VectorWithStartPos::getMaxX() const
{
  if (x < 0)
  {
    return startX_;
  }
  else
  {
    return startX_ + x;
  }
}

int VectorWithStartPos::getMaxY() const
{
  if (y < 0)
  {
    return startY_;
  }
  else
  {
    return startY_ + y;
  }
}

//
// Created by fbas on 13/06/25.
//

#include "VectorWithStartPos.h"

#include "Utility/ConfigController.h"

VectorWithStartPos::VectorWithStartPos(int x, int y, const Vector& vector) : Vector(vector), startX_(x), startY_(y)
{
}

bool VectorWithStartPos::isSameVector(const VectorWithStartPos &other) const
{
  int startXDiff = std::abs(startX_ - other.startX_);
  int startYDiff = std::abs(startY_ - other.startY_);
  int xDiff = std::abs(x - other.x);
  int yDiff = std::abs(y - other.y);

  if (startXDiff > ConfigController::getConfigInt("MaxDiffInSameCourseObject"))
  {
    return false;
  }
  if (startYDiff > ConfigController::getConfigInt("MaxDiffInSameCourseObject"))
  {
    return false;
  }
  if (xDiff > ConfigController::getConfigInt("MaxDiffInSameCourseObject"))
  {
    return false;
  }
  if (yDiff > ConfigController::getConfigInt("MaxDiffInSameCourseObject"))
  {
    return false;
  }
  return true;
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

//
// Created by fbas on 13/06/25.
//

#include "BlockingObject.h"

BlockingObject::BlockingObject(int x, int y, Vector vector) : x_(x), y_(y), vector_(vector)
{
  x_ = x;
}

int BlockingObject::x() const
{
  return x_;
}

int BlockingObject::y() const
{
  return y_;
}

Vector BlockingObject::vector() const
{
  return vector_;
}

Vector BlockingObject::minimalPoints() const
{
  return {getLowestX(),getLowestY()};
}

Vector BlockingObject::maxPoints() const
{
  return {getMaxX(),getMaxY()};
}

int BlockingObject::getLowestX() const
{
  if (vector_.x > 0)
  {
    return x_;
  }
  else
  {
    return x_ + vector_.x;
  }
}

int BlockingObject::getLowestY() const
{
  if (vector_.y > 0)
  {
    return y_;
  }
  else
  {
    return y_ + vector_.y;
  }
}

int BlockingObject::getMaxX() const
{
  if (vector_.x < 0)
  {
    return y_;
  }
  else
  {
    return y_ + vector_.y;
  }
}

int BlockingObject::getMaxY() const
{
  if (vector_.y < 0)
  {
    return y_;
  }
  else
  {
    return y_ + vector_.y;
  }
}

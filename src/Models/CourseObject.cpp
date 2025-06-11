//
// Created by Elias Aggergaard Larsen on 01/05/2025.
//

#include "CourseObject.h"

#include <utility>

CourseObject::CourseObject(int x1, int y1, int x2, int y2, std::string name) : x1_(x1), y1_(y1), x2_(x2), y2_(y2),
                                                                               name_(std::move(name))
{
}

int CourseObject::x1() const
{
  return x1_;
}

int CourseObject::y1() const
{
  return y1_;
}

int CourseObject::x2() const
{
  return x2_;
}

int CourseObject::y2() const
{
  return y2_;
}

std::string CourseObject::name() const
{
  return name_;
}

void CourseObject::shiftX(int x)
{
  x1_ += x;
  x2_ += x;
}

void CourseObject::shiftY(int y)
{
  y1_ += y;
  y2_ += y;
}

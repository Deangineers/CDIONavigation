//
// Created by Elias Aggergaard Larsen on 01/05/2025.
//

#include "CourseObject.h"

#include <utility>

#include "../Controllers/MathUtil.h"
#include "Utility/ConfigController.h"

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

bool CourseObject::operator==(const CourseObject& other) const
{
  if (this->x1() > other.x1() + 10 || this->x1() < other.x1() - 10)
    return false;

  if (this->x2() > other.x2() + 10 || this->x2() < other.x2() - 10)
    return false;

  if (this->y1() > other.y1() + 10 || this->y1() > other.y1() - 10)
    return false;

  if (this->y2() > other.y2() + 10 || this->y2() > other.y2() - 10)
    return false;

  if (this->name() != other.name())
    return false;

  return true;
}

bool CourseObject::courseObjectWithinValidRange(const CourseObject* other) const
{
  double distance = MathUtil::calculateVectorToObject(this, other).getLength();
  return distance < ConfigController::getConfigInt("MaxDiffInSameCourseObject");
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

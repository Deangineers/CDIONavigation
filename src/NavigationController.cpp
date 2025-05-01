//
// Created by Elias Aggergaard Larsen on 01/05/2025.
//

#include "NavigationController.h"
#include <cmath>

std::string NavigationController::navigate()
{
  return "null";
}

void NavigationController::addCourseObject(std::unique_ptr<CourseObject>&& courseObject)
{
  std::string name = courseObject->name();

  if (name == "ball")
  {
    ballVector_.push_back(std::move(courseObject));
  }
  else if (name == "egg")
  {
    eggVector_.push_back(std::move(courseObject));
  }
  else if (name == "goal")
  {
    goal_ = std::move(courseObject);
  }
  else if (name == "robotFront")
  {
    robotFront_ = std::move(courseObject);
  }
  else if (name == "robotBack")
  {
    robotBack_ = std::move(courseObject);
  }
  else
  {
    throw std::runtime_error("Invalid courseObject name" + name);
  }
}

void NavigationController::clearObjects()
{
  ballVector_.clear();
  eggVector_.clear();
  goal_ = nullptr;
  robotFront_ = nullptr;
  robotBack_ = nullptr;
}

std::pair<int, int> NavigationController::calculateVectorToObject(const CourseObject* courseObject) const
{
  const int objectWidth = courseObject->x2() - courseObject->x1();
  const int objectLength = courseObject->y2() - courseObject->y1();

  const int middleX = std::max(courseObject->x1() - objectWidth, courseObject->x2() - objectWidth);
  const int middleY = std::max(courseObject->y1() - objectLength, courseObject->y2() - objectLength);

  return std::make_pair(robotFront_->x1() - middleX, robotFront_->x1() - middleY);
}

double NavigationController::calculateAngleDifferenceBetweenVectors(const std::pair<int, int>& firstVector,
  const std::pair<int, int>& secondVector)
{
  int dotProduct = firstVector.first * secondVector.first + firstVector.second * secondVector.second;
  const double vector1Length = std::sqrt(firstVector.first * firstVector.first + firstVector.second * firstVector.second);
  const double vector2Length = std::sqrt(secondVector.first * secondVector.first + secondVector.second * secondVector.second);
  if (vector1Length == 0.0 || vector2Length == 0.0)
  {
    return 0.0;
  }

  double cosTheta = dotProduct / (vector1Length * vector2Length);
  if (cosTheta < -1.0) cosTheta = -1.0;
  if (cosTheta > 1.0) cosTheta = 1.0;

  double angleRadians = std::acos(cosTheta);
  double angleDegrees = angleRadians * (180.0 / M_PI);

  return angleDegrees;
}

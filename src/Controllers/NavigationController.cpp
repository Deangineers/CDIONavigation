//
// Created by Elias Aggergaard Larsen on 01/05/2025.
//

#include "NavigationController.h"
#include "../Models/JourneyModel.h"
#include <cmath>

std::unique_ptr<JourneyModel> NavigationController::calculateDegreesAndDistanceToObject()
{
  bool toCollectBalls = false;
  const CourseObject* objectToPathTowards = nullptr;
  if (ballVector_.empty() || ballsInRobot_ == robotBallCapacity_)
  {
    if (goal_ != nullptr)
    {
      objectToPathTowards = goal_.get();
      toCollectBalls = false;
    }
  }
  else
  {
    objectToPathTowards = findClosestBall();
    toCollectBalls = true;
  }
  if (objectToPathTowards == nullptr)
  {
    return nullptr;
  }
  const auto objectVector = calculateVectorToObject(objectToPathTowards);
  const std::pair robotVector = {robotFront_->x1() - robotBack_->x1(), robotFront_->y1() - robotBack_->y1()};
  const double angle = calculateAngleDifferenceBetweenVectors(robotVector,objectVector);
  const double distanceToObject = std::sqrt(objectVector.first * objectVector.first + objectVector.second * objectVector.second);
  return std::make_unique<JourneyModel>(distanceToObject, angle, toCollectBalls);
}

void NavigationController::addCourseObject(std::unique_ptr<CourseObject>&& courseObject)
{
  const std::string name = courseObject->name();
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

const CourseObject* NavigationController::findClosestBall() const
{
  if (ballVector_.empty())
  {
    return nullptr;
  }

  double shortestDistance = INT32_MAX;
  CourseObject* closestBall = nullptr;
  for (const auto& ball : ballVector_)
  {
    auto vectorToBall = calculateVectorToObject(ball.get());
    double length = std::sqrt(vectorToBall.first * vectorToBall.first + vectorToBall.second * vectorToBall.second);
    if (length  < shortestDistance)
    {
      shortestDistance = length;
      closestBall = ball.get();
    }
  }
  return closestBall;
}

std::pair<int, int> NavigationController::calculateVectorToObject(const CourseObject* courseObject) const
{
  const int objectWidth = courseObject->x2() - courseObject->x1();
  const int objectLength = courseObject->y2() - courseObject->y1();

  const int middleX = std::max(courseObject->x1() - objectWidth, courseObject->x2() - objectWidth);
  const int middleY = std::max(courseObject->y1() - objectLength, courseObject->y2() - objectLength);

  return std::make_pair(middleX - robotFront_->x1(), middleY - robotFront_->y1());
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
  double crossProduct = firstVector.first * secondVector.second - firstVector.second * secondVector.first;

  if (crossProduct > 0.0)
  {
    angleRadians = -angleRadians;
  }

  double angleDegrees = angleRadians * (180.0 / M_PI);

  return angleDegrees;
}

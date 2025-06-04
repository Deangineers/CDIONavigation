//
// Created by Elias Aggergaard Larsen on 01/05/2025.
//

#include "NavigationController.h"
#include "../Models/JourneyModel.h"
#include <cmath>

std::unique_ptr<JourneyModel> NavigationController::calculateDegreesAndDistanceToObject()
{
  if (robotFront_ == nullptr || robotBack_ == nullptr)
  {
    return nullptr;
  }
  bool toCollectBalls = false;
  const CourseObject* objectToPathTowards = nullptr;
  auto objectVector = std::make_pair(0,0);
  if (courseObject_ == nullptr)
  {
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

    auto courseObject = std::make_unique<CourseObject>(currentX_,currentY_,currentX_,currentY_,"");
    objectVector = calculateVectorToObject(objectToPathTowards);
    while (checkCollisionOnRoute(objectToPathTowards, objectVector))
    {
      courseObject = std::make_unique<CourseObject>(currentX_,currentY_,currentX_,currentY_,"");
      objectToPathTowards = courseObject.get();
      objectVector = calculateVectorToObject(courseObject.get());
      if (currentX_ == safeXLeft_ && currentY_ == safeYTop_)
      {
        currentX_++;
      }
      if (currentX_ == safeXLeft_)
      {
        currentY_--;
      }

      else if (currentY_ == safeYTop_ && currentX_ == safeXRight_)
      {
        currentY_++;
      }
      else if (currentY_ == safeYTop_)
      {
        currentX_++;
      }

      else if (currentX_ == safeXRight_ && currentY_ == safeYBot_)
      {
        currentX_--;
      }
      else if (currentX_ == safeXRight_)
      {
        currentY_++;
      }
      else if (currentY_ == safeYBot_)
      {
        currentX_--;
      }
      courseObject_ = std::move(courseObject);
    }
    if (courseObject_ == nullptr)
    {
      courseObject_ = std::make_unique<CourseObject>(objectToPathTowards->x1(),objectToPathTowards->y1(),objectToPathTowards->x2(),objectToPathTowards->y2(),objectToPathTowards->name());
    }
  }
  else
  {
    objectVector = calculateVectorToObject(courseObject_.get());
  }
  const std::pair robotVector = {robotFront_->x1() - robotBack_->x1(), robotFront_->y1() - robotBack_->y1()};
  const double angle = calculateAngleDifferenceBetweenVectors(robotVector,objectVector);
  const double distanceToObject = std::sqrt(objectVector.first * objectVector.first + objectVector.second * objectVector.second);
  if (distanceToObject < 10)
  {
    courseObject_ = nullptr;
  }
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
    blockingObject_.push_back(std::move(courseObject));
  }
  else if (name == "smallgoal")
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
  else if (name == "blockedObject")
  {
    blockingObject_.push_back(std::move(courseObject));
  }
  else if (name == "biggoal")
  {
    // Ignore this case ;)
  }
  else
  {
    throw std::runtime_error("Invalid courseObject name" + name);
  }
}

void NavigationController::clearObjects()
{
  ballVector_.clear();
  blockingObject_.clear();
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

bool NavigationController::checkCollisionOnRoute(const CourseObject* target, const std::pair<int,int>& targetVector) const
{
  if (!robotFront_ || !target) return false;

  int startX = robotFront_->x1();
  int startY = robotFront_->y1();
  int endX = startX + targetVector.first;
  int endY = startY + targetVector.second;

  double length = std::sqrt(targetVector.first * targetVector.first + targetVector.second * targetVector.second);
  if (length == 0.0) return false;

  double offsetX = -(targetVector.second / length) * (robotWidth_ / 2.0);
  double offsetY =  (targetVector.first / length) * (robotWidth_ / 2.0);

  double topX = std::max(startX + offsetX,endX + offsetX);
  double topY = std::max(startY + offsetY,endY + offsetY);
  double bottomX = std::min(startX + offsetX,endX + offsetX);
  double bottomY = std::min(startY + offsetY,endY + offsetY);

  for (const auto& blocker : blockingObject_)
  {
    double blockerMaxX = std::max(blocker->x1(), blocker->x2());
    double blockerMaxY = std::max(blocker->y1(), blocker->y2());
    double blockerMinX = std::min(blocker->x1(), blocker->x2());
    double blockerMinY = std::min(blocker->y1(), blocker->y2());

    if (blockerMaxX < bottomX || blockerMaxY < bottomY)
    {
      continue;
    }
    if (blockerMinX > topX || blockerMinY > topY)
    {
      continue;
    }
    return true;
  }

  return false;
}


//
// Created by Elias Aggergaard Larsen on 01/05/2025.
//

#include "NavigationController.h"

#include <algorithm>
#include <iostream>
#include <opencv2/core/matx.hpp>

#include "../Models/JourneyModel.h"
#include "Utility/ConfigController.h"
#include "cmath"
#include "MathUtil.h"
#include "Utility/Utility.h"

void NavigationController::addCourseObject(std::unique_ptr<CourseObject>&& courseObject)
{
  const std::string name = courseObject->name();
  if (name == "ball")
  {
    ballVector_.push_back(std::move(courseObject));
  }
  else if (name == "robotFront")
  {
    int x1 = courseObject->x1() + ConfigController::getConfigInt("RobotFrontOffsetX");
    int y1 = courseObject->y1() + ConfigController::getConfigInt("RobotFrontOffsetY");
    int x2 = courseObject->x2() + ConfigController::getConfigInt("RobotFrontOffsetX");
    int y2 = courseObject->y2() + ConfigController::getConfigInt("RobotFrontOffsetY");

    x1 += x2 - x1;
    y1 += y2 - y1;

    robotFront_ = std::make_unique<CourseObject>(x1, y1, x1, y1, "robotFront");
  }
  else if (name == "robotBack")
  {
    int x1 = courseObject->x1() + ConfigController::getConfigInt("RobotBackOffsetX");
    int y1 = courseObject->y1() + ConfigController::getConfigInt("RobotBackOffsetY");
    int x2 = courseObject->x2() + ConfigController::getConfigInt("RobotBackOffsetX");
    int y2 = courseObject->y2() + ConfigController::getConfigInt("RobotBackOffsetY");

    x1 += x2 - x1;
    y1 += y2 - y1;

    robotBack_ = std::make_unique<CourseObject>(x1, y1, x1, y1, "robotBack");
  }
  else if (name == "blockedObject" || name == "egg")
  {
    blockingObjects_.push_back(std::move(courseObject));
  }
  else if (name == "goal")
  {
    if (goal_ == nullptr)
    {
      goal_ = std::move(courseObject);
    }
    else
    {
      double sizeOfCurrentGoal = std::sqrt(
        (goal_->x2() - goal_->x1()) * (goal_->x2() - goal_->x1()) + (goal_->y2() - goal_->y1()) * (goal_->y2() - goal_->
          y1()));
      double sizeOfProposedGoal = std::sqrt(
        (courseObject->x2() - courseObject->x1()) * (courseObject->x2() - courseObject->x1()) + (courseObject->y2() -
          courseObject->y1()) * (courseObject->y2() - courseObject->y1()));
      if (sizeOfProposedGoal > sizeOfCurrentGoal)
      {
        goal_ = std::move(courseObject);
      }
    }
  }
  else
  {
    throw std::runtime_error("Invalid courseObject name" + name);
  }
}

void NavigationController::clearObjects()
{
  ballVector_.clear();
  blockingObjects_.clear();
  goal_ = nullptr;
  robotFront_ = nullptr;
  robotBack_ = nullptr;
}

std::unique_ptr<JourneyModel> NavigationController::calculateDegreesAndDistanceToObject()
{
  if (robotFront_ == nullptr || robotBack_ == nullptr)
  {
    Utility::appendToFile("log.txt", "No Robot\n");
    return nullptr;
  }
  MathUtil::correctCourseObjectForHeightOffset(robotBack_.get(), robotFront_.get());
  bool toCollectBalls = true;
  auto objectVector = Vector(0, 0);
  if (ballVector_.empty())
  {
    objectVector = navigateToGoal();
    toCollectBalls = false;
  }
  else
  {
    removeBallsInsideRobot();
    removeBallsOutsideCourse();
    objectVector = findClosestBall();
    toCollectBalls = true;
  }

  if (objectVector.isNullVector())
  {
    Utility::appendToFile("log.txt", "objectVector = {0,0}, firstCheck\n");
    return nullptr;
  }
  objectVector = handleCollision(objectVector);

  if (objectVector.isNullVector())
  {
    Utility::appendToFile("log.txt", "objectVector = {0,0}, secondCheck\n");
  }

  return makeJourneyModel(objectVector, toCollectBalls);
}

std::unique_ptr<JourneyModel> NavigationController::makeJourneyModel(const Vector& objectVector,
                                                                     bool toCollectBalls) const
{
  const Vector robotVector = {robotFront_->x1() - robotBack_->x1(), robotFront_->y1() - robotBack_->y1()};
  const double angle = MathUtil::calculateAngleDifferenceBetweenVectors(robotVector, objectVector);

  auto vectorToRobotBack = MathUtil::calculateVectorToObject(robotFront_.get(), robotBack_.get());
  double distanceInCm = objectVector.getLength() * ((static_cast<double>(ConfigController::getConfigInt(
      "RobotLengthInMM")) /
    10) / vectorToRobotBack.getLength());
  return std::make_unique<JourneyModel>(distanceInCm, angle, toCollectBalls);
}

void NavigationController::removeBallsOutsideCourse()
{
  if (blockingObjects_.empty())
  {
    return;
  }
  auto topLeftCornerLambda = [](const std::unique_ptr<CourseObject>& a, const std::unique_ptr<CourseObject>& b) -> bool
  {
    return a->x1() < b->x1() && a->y1() < b->y1();
  };
  auto topLeftCorner = std::max_element(blockingObjects_.begin(), blockingObjects_.end(), topLeftCornerLambda)->get();
  auto bottomRightCornerLambda = [](const std::unique_ptr<CourseObject>& a,
                                    const std::unique_ptr<CourseObject>& b) -> bool
  {
    return a->x1() > b->x1() && a->y1() > b->y1();
  };

  auto bottomRightCorner = std::max_element(blockingObjects_.begin(), blockingObjects_.end(), bottomRightCornerLambda)->
    get();

  auto deletionLambda = [topLeftCorner,bottomRightCorner](const std::unique_ptr<CourseObject>& a) -> bool
  {
    return a->x1() > topLeftCorner->x1() && bottomRightCorner->x1() > a->x1() && a->y1() > topLeftCorner->y1() &&
      bottomRightCorner->y1() > a->y1();
  };

  std::erase_if(ballVector_, deletionLambda);
}

void NavigationController::removeBallsInsideRobot()
{
  double topX = std::min(robotFront_->x1(), robotBack_->x1());
  double topY = std::min(robotFront_->y1(), robotBack_->y1());
  double bottomX = std::max(robotFront_->x2(), robotBack_->x2());
  double bottomY = std::max(robotFront_->y2(), robotBack_->y2());

  std::erase_if(ballVector_, [topX, topY, bottomX, bottomY](const std::unique_ptr<CourseObject>& ball)
  {
    double ballTopX = std::min(ball->x1(), ball->x2());
    double ballTopY = std::min(ball->y1(), ball->y2());
    double ballBottomX = std::max(ball->x1(), ball->x2());
    double ballBottomY = std::max(ball->y1(), ball->y2());

    return ballBottomX > topX && ballTopX < bottomX && ballBottomY > topY && ballTopY < bottomY;
  });
}

Vector NavigationController::navigateToGoal() const
{
  Vector goal(-1, -1);
  if (ConfigController::getConfigBool("goalIsLeft"))
  {
    goal = navigateToLeftGoal();
  }
  else
  {
    goal = navigateToRightGoal();
  }

  if (goal.x == -1)
  {
    return {0, 0};
  }

  int targetX;
  if (goal.x > ConfigController::getConfigInt("middleXOnAxis"))
  {
    targetX = goal.x - ConfigController::getConfigInt("distanceToGoal");
  }
  else
  {
    targetX = goal.x + ConfigController::getConfigInt("distanceToGoal");
  }
  auto courseObject = CourseObject(targetX, goal.y, targetX, goal.y, "goal");

  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());
  return MathUtil::calculateVectorToObject(&robotMiddle, &courseObject);
}

Vector NavigationController::findClosestBall() const
{
  if (ballVector_.empty())
  {
    return {0, 0};
  }

  double shortestDistance = INT32_MAX;
  CourseObject* closestBall = nullptr;
  for (const auto& ball : ballVector_)
  {
    auto vectorToBall = MathUtil::calculateVectorToObject(robotFront_.get(), ball.get());
    double length = std::sqrt(vectorToBall.x * vectorToBall.x + vectorToBall.y * vectorToBall.y);
    if (length < shortestDistance)
    {
      shortestDistance = length;
      closestBall = ball.get();
    }
  }
  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());
  return MathUtil::calculateVectorToObject(&robotMiddle, closestBall);
}

Vector NavigationController::navigateToLeftGoal() const
{
  int minY = -1;
  int maxX = -1;
  int maxY = -1;

  for (const auto& object : blockingObjects_)
  {
    if (object->x1() > maxX)
      maxX = object->x1();

    if (object->y2() > maxY)
      maxY = object->y2();

    if (object->y1() < minY)
      minY = object->y1();
  }

  int middleY = (minY + maxY) / 2;
  return Vector(maxX, middleY);
}

Vector NavigationController::navigateToRightGoal() const
{
  int minY = -1;
  int minX = -1;
  int maxY = -1;

  for (const auto& object : blockingObjects_)
  {
    if (object->x1() < minX)
      minX = object->x1();

    if (object->y2() > maxY)
      maxY = object->y2();

    if (object->y1() < minY)
      minY = object->y1();
  }

  int middleY = (minY + maxY) / 2;
  return Vector(minX, middleY);
}

Vector NavigationController::handleCollision(Vector objectVector)
{
  int startX = currentX_;
  int startY = currentY_;
  while (checkCollisionOnRoute(objectVector))
  {
    auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());
    auto courseObject = CourseObject(currentX_, currentY_, currentX_, currentY_, "safeSpot");
    Utility::appendToFile("log.txt", "Redirecting to safe spot:\n");
    objectVector = MathUtil::calculateVectorToObject(&robotMiddle, &courseObject);
    if (currentX_ == ConfigController::getConfigInt("safeXLeft") && currentY_ ==
      ConfigController::getConfigInt("safeYTop"))
    {
      currentX_++;
    }
    if (currentX_ == ConfigController::getConfigInt("safeXLeft"))
    {
      currentY_--;
    }

    else if (currentY_ == ConfigController::getConfigInt("safeYTop") && currentX_ ==
      ConfigController::getConfigInt("safeXRight"))
    {
      currentY_++;
    }
    else if (currentY_ == ConfigController::getConfigInt("safeYTop"))
    {
      currentX_++;
    }

    else if (currentX_ == ConfigController::getConfigInt("safeXRight") && currentY_ ==
      ConfigController::getConfigInt("safeYBot"))
    {
      currentX_--;
    }
    else if (currentX_ == ConfigController::getConfigInt("safeXRight"))
    {
      currentY_++;
    }
    else if (currentY_ == ConfigController::getConfigInt("safeYBot"))
    {
      currentX_--;
    }
    if (currentX_ == startX && currentY_ == startY)
    {
      Utility::appendToFile("log.txt", "Broke out of collision, no safe places\n");
      return {0, 0};
    }
  }
  Utility::appendToFile(
    "log.txt",
    "Navigating to vector: " + std::to_string(objectVector.x) + ", " + std::to_string(objectVector.y) + "\n");
  return objectVector;
}

Vector NavigationController::getVectorForObjectNearWall(const CourseObject* courseObject) const
{
  if (courseObject == nullptr)
  {
    return {0, 0};
  }
  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());

  auto closestVectors = getVectorsForClosestBlockingObjects(courseObject);
  Utility::appendToFile(
    "log.txt", "ClosestVectors: " + closestVectors.first.toString() + " | " + closestVectors.second.toString() + "\n");
  if (closestVectors.second.getSmallestValue() < ConfigController::getConfigInt("DistanceToWallBeforeHandling"))
  {
    auto vectorToWall = closestVectors.first;
    if (vectorToWall.getSmallestValue() > ConfigController::getConfigInt("DistanceToWallBeforeHandling"))
    {
      return MathUtil::calculateVectorToObject(&robotMiddle, courseObject);
    }
    // 1 wall
    auto localCourseObject = CourseObject(*courseObject);
    int singleWallShiftDiff = ConfigController::getConfigInt("SingleWallShiftDiff");
    if (vectorToWall.x == vectorToWall.getSmallestValue())
    {
      localCourseObject.shiftX(vectorToWall.x > 0 ? -singleWallShiftDiff : singleWallShiftDiff);
    }
    else
    {
      localCourseObject.shiftY(vectorToWall.y > 0 ? -singleWallShiftDiff : singleWallShiftDiff);
    }


    auto vectorToDiffPoint = MathUtil::calculateVectorToObject(&robotMiddle, &localCourseObject);
    if (vectorToDiffPoint.getLength() < ConfigController::getConfigInt("DistanceToShiftedPointBeforeTurning"))
    {
      return MathUtil::calculateVectorToObject(&robotMiddle, courseObject);
    }
    return MathUtil::calculateVectorToObject(&robotMiddle, &localCourseObject);
  }
  else
  {
    // Corner
  }
  return {0, 0};
}

std::pair<Vector, Vector> NavigationController::getVectorsForClosestBlockingObjects(
  const CourseObject* courseObject) const
{
  auto returnPair = std::make_pair(Vector(INT8_MAX,INT8_MAX), Vector(INT8_MAX,INT8_MAX));
  for (const auto& blockingObject : blockingObjects_)
  {
    auto vector = MathUtil::calculateVectorToObject(courseObject, blockingObject.get());
    if (not returnPair.first.hasSmallerValueThan(vector))
    {
      returnPair.second = returnPair.first;
      returnPair.first = vector;
    }
    else if (not returnPair.second.hasSmallerValueThan(vector))
    {
      returnPair.second = vector;
    }
  }
  return returnPair;
}

bool NavigationController::checkCollisionOnRoute(const Vector& targetVector) const
{
  if (!robotFront_) return false;

  int startX = robotFront_->x1();
  int startY = robotFront_->y1();
  int endX = startX + targetVector.x;
  int endY = startY + targetVector.y;

  double length = std::sqrt(targetVector.x * targetVector.x + targetVector.y * targetVector.y);
  if (length == 0.0) return false;

  double offsetX = -(targetVector.y / length) * (robotWidth_ / 2.0);
  double offsetY = (targetVector.x / length) * (robotWidth_ / 2.0);

  double topX = std::max(startX + offsetX, endX + offsetX);
  double topY = std::max(startY + offsetY, endY + offsetY);
  double bottomX = std::min(startX + offsetX, endX + offsetX);
  double bottomY = std::min(startY + offsetY, endY + offsetY);

  for (const auto& blocker : blockingObjects_)
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


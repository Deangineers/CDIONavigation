//
// Created by Elias Aggergaard Larsen on 01/05/2025.
//

#include "NavigationController.h"

#include <algorithm>
#include <iostream>

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
  bool toCollectBalls = true;
  CourseObject* objectToPathTowards = nullptr;
  auto objectVector = std::make_pair(0, 0);
  if (ballVector_.empty() || totalBalls_ - ballVector_.size() == robotBallCapacity_)
  {
    navigateToGoal(&objectToPathTowards, toCollectBalls);
  }
  else
  {
    removeBallsInsideRobot();
    removeBallsOutsideCourse();
    objectToPathTowards = findClosestBall();
    toCollectBalls = true;
  }

  if (objectToPathTowards == nullptr)
  {
    Utility::appendToFile("log.txt", "objectToPathTowards = nullptr, firstCheck\n");
    return nullptr;
  }
  handleCollision(&objectToPathTowards);
  CourseObject* robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());
  objectVector = MathUtil::calculateVectorToObject(robotMiddle, objectToPathTowards);

  if (objectToPathTowards == nullptr)
  {
    Utility::appendToFile("log.txt", "objectToPathTowards = nullptr, secondCheck\n");
  }
  Utility::appendToFile(
    "log.txt",
    "Target: " + objectToPathTowards->name() + " " + std::to_string(objectToPathTowards->x1()) + ", " +
    std::to_string(objectToPathTowards->y1()) + "\n");
  Utility::appendToFile(
    "log.txt",
    robotFront_->name() + ": " + std::to_string(robotFront_->x1()) + ", " + std::to_string(robotFront_->y1()) + "\n");
  Utility::appendToFile(
    "log.txt",
    robotBack_->name() + ": " + std::to_string(robotBack_->x1()) + ", " + std::to_string(robotBack_->y1()) + "\n");

  return makeJourneyModel(objectVector, toCollectBalls);
}

std::unique_ptr<JourneyModel> NavigationController::makeJourneyModel(const std::pair<int, int>& objectVector,
                                                                     bool toCollectBalls)
{
  const std::pair robotVector = {robotFront_->x1() - robotBack_->x1(), robotFront_->y1() - robotBack_->y1()};
  const double angle = MathUtil::calculateAngleDifferenceBetweenVectors(robotVector, objectVector);
  const double distanceToObject = std::sqrt(
    objectVector.first * objectVector.first + objectVector.second * objectVector.second);

  auto vectorToRobotBack = MathUtil::calculateVectorToObject(robotFront_.get(), robotBack_.get());
  double distanceInCm = distanceToObject * ((static_cast<double>(ConfigController::getConfigInt("RobotLengthInMM")) /
    10) / std::sqrt(
    vectorToRobotBack.first * vectorToRobotBack.first + vectorToRobotBack.second * vectorToRobotBack.second));
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

void NavigationController::navigateToGoal(CourseObject** objectToPathTowards, bool& toCollectBalls)
{
  if (goal_ != nullptr)
  {
    int targetX;
    int targetY = goal_->y1();
    if (goal_->x1() > ConfigController::getConfigInt("middleXOnAxis"))
    {
      targetX = goal_->x1() - ConfigController::getConfigInt("distanceToGoal");
      targetY = goal_->y1();
    }
    else
    {
      targetX = goal_->x1() + ConfigController::getConfigInt("distanceToGoal");
    }
    goal_ = std::make_unique<CourseObject>(targetX, goal_->y1(), targetX, goal_->y2(), "smallgoal");
    *objectToPathTowards = goal_.get();
    toCollectBalls = false;
  }
}

CourseObject* NavigationController::findClosestBall() const
{
  if (ballVector_.empty())
  {
    return nullptr;
  }

  double shortestDistance = INT32_MAX;
  CourseObject* closestBall = nullptr;
  for (const auto& ball : ballVector_)
  {
    auto vectorToBall = MathUtil::calculateVectorToObject(robotFront_.get(), ball.get());
    double length = std::sqrt(vectorToBall.first * vectorToBall.first + vectorToBall.second * vectorToBall.second);
    if (length < shortestDistance)
    {
      shortestDistance = length;
      closestBall = ball.get();
    }
  }
  return closestBall;
}

void NavigationController::handleCollision(CourseObject** objectToPathTowards)
{
  safeSpotPointer_ = std::make_unique<CourseObject>((*objectToPathTowards)->x1(), (*objectToPathTowards)->y1(),
                                                    (*objectToPathTowards)->x2(), (*objectToPathTowards)->y2(),
                                                    (*objectToPathTowards)->name());
  auto objectVector = MathUtil::calculateVectorToObject(robotFront_.get(), safeSpotPointer_.get());
  while (checkCollisionOnRoute(*objectToPathTowards, objectVector))
  {
    std::cout << "Collision Detected, ignoring for now\n";
    safeSpotPointer_ = std::make_unique<CourseObject>(currentX_, currentY_, currentX_, currentY_, "safeSpot");
    *objectToPathTowards = safeSpotPointer_.get();
    objectVector = MathUtil::calculateVectorToObject(robotFront_.get(), *objectToPathTowards);
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
  }
}

bool NavigationController::checkCollisionOnRoute(const CourseObject* target,
                                                 const std::pair<int, int>& targetVector) const
{
  if (!robotFront_ || !target) return false;

  int startX = robotFront_->x1();
  int startY = robotFront_->y1();
  int endX = startX + targetVector.first;
  int endY = startY + targetVector.second;

  double length = std::sqrt(targetVector.first * targetVector.first + targetVector.second * targetVector.second);
  if (length == 0.0) return false;

  double offsetX = -(targetVector.second / length) * (robotWidth_ / 2.0);
  double offsetY = (targetVector.first / length) * (robotWidth_ / 2.0);

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


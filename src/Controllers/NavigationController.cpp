//
// Created by Elias Aggergaard Larsen on 01/05/2025.
//

#include "NavigationController.h"

#include <algorithm>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/matx.hpp>

#include "../Models/JourneyModel.h"
#include "Utility/ConfigController.h"
#include "cmath"
#include "MainController.h"
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

    x1 = (x2 + x1)/2;
    y1 = (y2 + y1)/2;

    robotFront_ = std::make_unique<CourseObject>(x1, y1, x1, y1, "robotFront");
  }
  else if (name == "robotBack")
  {
    int x1 = courseObject->x1() + ConfigController::getConfigInt("RobotBackOffsetX");
    int y1 = courseObject->y1() + ConfigController::getConfigInt("RobotBackOffsetY");
    int x2 = courseObject->x2() + ConfigController::getConfigInt("RobotBackOffsetX");
    int y2 = courseObject->y2() + ConfigController::getConfigInt("RobotBackOffsetY");

    x1 = (x2 + x1)/2;
    y1 = (y2 + y1)/2;

    robotBack_ = std::make_unique<CourseObject>(x1, y1, x1, y1, "robotBack");
  }
  else if (name == "egg")
  {
    auto horizontalVector = Vector(courseObject->x2() - courseObject->x1(), 0);
    auto verticalVector = Vector(0,courseObject->y2() - courseObject->y1());

    auto topBar = std::make_unique<BlockingObject>(courseObject->x1(),courseObject->y1(),horizontalVector);
    auto bottomBar = std::make_unique<BlockingObject>(courseObject->x1(),courseObject->y2(),horizontalVector);

    auto leftBar = std::make_unique<BlockingObject>(courseObject->x1(),courseObject->y1(),verticalVector);
    auto rightBar = std::make_unique<BlockingObject>(courseObject->x2(),courseObject->y1(),verticalVector);

    blockingObjects_.push_back(std::move(topBar));
    blockingObjects_.push_back(std::move(bottomBar));
    blockingObjects_.push_back(std::move(leftBar));
    blockingObjects_.push_back(std::move(rightBar));
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

void NavigationController::addBlockingObject(std::unique_ptr<BlockingObject>&& blockingObject)
{
  blockingObjects_.push_back(std::move(blockingObject));
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

  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(),robotFront_.get());
  cv::arrowedLine(*MainController::getFrame(), {robotMiddle.x1(),robotMiddle.y1()}, {robotMiddle.x1() + objectVector.x, robotMiddle.y1() + objectVector.y}, cv::Scalar(255, 0, 255), 1,
                                  cv::LINE_AA, 0, 0.01);

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

  int minX = INT_MAX;
  int minY = INT_MAX;
  int highX = INT_MIN;
  int highY = INT_MIN;

  for (const auto& blockingObject : blockingObjects_)
  {
    auto max = blockingObject->maxPoints();
    auto min = blockingObject->minimalPoints();

    if (min.x < minX)
    {
      minX = min.x;
    }
    if (min.y < minY)
    {
      minY = min.y;
    }
    if (highX < max.x)
    {
      highX = max.x;
    }
    if (highY < max.y)
    {
      highY = max.y;
    }
  }

  auto deletionLambda = [minX,minY,highX,highY](const std::unique_ptr<CourseObject>& a) -> bool
  {
    return (a->x1() < minX || highX > a->x2()) && (a->y1() < minY ||
      highY > a->y2());
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

  Utility::appendToFile(
    "log.txt",
    "Navigating to Goal: " + std::to_string(courseObject.x1()) + ", " + std::to_string(courseObject.y1()) + "\n");

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
    auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());
    auto vectorToBall = MathUtil::calculateVectorToObject(&robotMiddle, ball.get());
    vectorToBall = getVectorForObjectNearWall(ball.get());
    if (vectorToBall.getLength() < shortestDistance)
    {
      shortestDistance = vectorToBall.getLength();
      closestBall = ball.get();
    }
  }
  if (closestBall != nullptr)
  {
    Utility::appendToFile(
      "log.txt",
      "Navigating to Ball: " + std::to_string(closestBall->x1()) + ", " + std::to_string(closestBall->y1()) + "\n");
  }
  else
  {
    Utility::appendToFile(
      "log.txt", "Navigating to Ball: BUT NO BALLS FOUND\n");
  }
  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());
  return MathUtil::calculateVectorToObject(&robotMiddle, closestBall);
}

Vector NavigationController::navigateToLeftGoal() const
{
  int minY = -1;
  int minX = -1;
  int maxY = -1;

  for (const auto& object : blockingObjects_)
  {
    const int x0 = object->x();
    const int y0 = object->y();

    const Vector v = object->vector();
    const int x1 = x0 + v.x;
    const int y1 = y0 + v.y;

    minX = std::min({minX, x0, x1});
    minY = std::min({minY, y0, y1});
    maxY = std::max({maxY, y0, y1});
  }

  const int middleY = (minY + maxY) / 2;
  return Vector{minX, middleY};
}

Vector NavigationController::navigateToRightGoal() const
{
  int minY = -1;
  int maxX = -1;
  int maxY = -1;

  for (const auto& object : blockingObjects_)
  {
    const int x0 = object->x();
    const int y0 = object->y();

    const Vector v = object->vector();
    const int x1 = x0 + v.x;
    const int y1 = y0 + v.y;

    maxX = std::max({maxX, x0, x1});
    minY = std::min({minY, y0, y1});
    maxY = std::max({maxY, y0, y1});
  }

  const int middleY = (minY + maxY) / 2;
  return Vector{maxX, middleY};
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
    "Collision Detected, moving to: " + std::to_string(objectVector.x) + ", " + std::to_string(objectVector.y) + "\n");
  return objectVector;
}

Vector NavigationController::getVectorForObjectNearWall(const CourseObject* courseObject) const
{
  if (courseObject == nullptr)
  {
    Utility::appendToFile("log.txt","getVectorForObjectNearWall(), courseObject is nullptr");
    return {0, 0};
  }
  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());

  auto closestVectors = getVectorsForClosestBlockingObjects(courseObject);
  Utility::appendToFile(
    "log.txt", "ClosestVectors: " + closestVectors.first.toString() + " | " + closestVectors.second.toString() + "\n");

  if (ConfigController::getConfigBool("showVectorsToWall"))
  {
    cv::arrowedLine(*MainController::getFrame(), {(courseObject->x1() + courseObject->x2())/2,(courseObject->y1() + courseObject->y2())/2}, {courseObject->x1() + closestVectors.first.x, courseObject->y1() + closestVectors.first.y}, cv::Scalar(0, 0, 255), 1,
                                cv::LINE_AA, 0, 0.01);
    cv::arrowedLine(*MainController::getFrame(), {(courseObject->x1() + courseObject->x2())/2,(courseObject->y1() + courseObject->y2())/2}, {courseObject->x1() + closestVectors.second.x, courseObject->y1() + closestVectors.second.y}, cv::Scalar(0, 0, 255), 1,
                                cv::LINE_AA, 0, 0.01);
  }

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
    auto startVector = Vector(blockingObject->x(),blockingObject->y());
    auto fromPointVector = Vector(courseObject->x1(),courseObject->y1());
    auto vector = blockingObject->vector().closestVectorFromPoint(startVector,fromPointVector);

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

bool segmentsIntersect(int x1, int y1, int x2, int y2,
                       int x3, int y3, int x4, int y4)
{
  auto orientation = [](int ax, int ay, int bx, int by, int cx, int cy) {
    int val = (by - ay) * (cx - bx) - (bx - ax) * (cy - by);
    if (val == 0) return 0;
    return (val > 0) ? 1 : 2;
  };

  int o1 = orientation(x1, y1, x2, y2, x3, y3);
  int o2 = orientation(x1, y1, x2, y2, x4, y4);
  int o3 = orientation(x3, y3, x4, y4, x1, y1);
  int o4 = orientation(x3, y3, x4, y4, x2, y2);

  if (o1 != o2 && o3 != o4) return true;

  auto onSegment = [](int ax, int ay, int bx, int by, int cx, int cy) {
    return cx >= std::min(ax, bx) && cx <= std::max(ax, bx) &&
           cy >= std::min(ay, by) && cy <= std::max(ay, by);
  };

  if (o1 == 0 && onSegment(x1, y1, x2, y2, x3, y3)) return true;
  if (o2 == 0 && onSegment(x1, y1, x2, y2, x4, y4)) return true;
  if (o3 == 0 && onSegment(x3, y3, x4, y4, x1, y1)) return true;
  if (o4 == 0 && onSegment(x3, y3, x4, y4, x2, y2)) return true;

  return false;
}

bool NavigationController::checkCollisionOnRoute(const Vector& targetVector) const
{
  if (!robotFront_)
  {
    return false;
  }

  int startX = (robotFront_->x1() + robotFront_->x2())/2;
  int startY = (robotFront_->y1() + robotFront_->y2())/2;
  int endX = startX + targetVector.x;
  int endY = startY + targetVector.y;

  double length = std::sqrt(targetVector.x * targetVector.x + targetVector.y * targetVector.y);
  if (length == 0.0) return false;

  double halfWidth = robotWidth_ / 2.0;

  double perpX = -(targetVector.y / length);
  double perpY = (targetVector.x / length);

  double offsetX = perpX * halfWidth;
  double offsetY = perpY * halfWidth;

  struct Segment {
    double x1, y1, x2, y2;
  };

  Segment center = Segment(startX,startY,endX,endY);
  Segment left{
    startX + offsetX, startY + offsetY,
    endX + offsetX, endY + offsetY
  };
  Segment right{
    startX - offsetX, startY - offsetY,
    endX - offsetX, endY - offsetY
  };

  for (const auto& blocker : blockingObjects_)
  {
    int bx1 = blocker->x();
    int by1 = blocker->y();
    int bx2 = bx1 + blocker->vector().x;
    int by2 = by1 + blocker->vector().y;

    if (segmentsIntersect(center.x1, center.y1, center.x2, center.y2, bx1, by1, bx2, by2) ||
        segmentsIntersect(left.x1, left.y1, left.x2, left.y2, bx1, by1, bx2, by2) ||
        segmentsIntersect(right.x1, right.y1, right.x2, right.y2, bx1, by1, bx2, by2))
    {
      return true;
    }
  }

  return false;
}


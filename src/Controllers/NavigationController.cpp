//
// Created by Elias Aggergaard Larsen on 01/05/2025.
//

#include "NavigationController.h"

#include <algorithm>
#include <iostream>
#include <ranges>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/matx.hpp>

#include "../Models/JourneyModel.h"
#include "Utility/ConfigController.h"
#include "cmath"
#include "MainController.h"
#include "MathUtil.h"
#include "../Models/VectorWithStartPos.h"
#include "Utility/Utility.h"
#include "../Models/Egg.h"

void NavigationController::addCourseObject(std::unique_ptr<CourseObject>&& courseObject)
{
  const std::string name = courseObject->name();
  if (name == "ball")
  {
    ballVector_.push_back(std::move(courseObject));
  }
  else if (name == "robotFront")
  {
    int x1 = courseObject->x1();
    int y1 = courseObject->y1();
    int x2 = courseObject->x2();
    int y2 = courseObject->y2();

    x1 = (x2 + x1) / 2;
    y1 = (y2 + y1) / 2;

    robotFront_ = std::make_unique<CourseObject>(x1, y1, x1, y1, "robotFront");
  }
  else if (name == "robotBack")
  {
    int x1 = courseObject->x1();
    int y1 = courseObject->y1();
    int x2 = courseObject->x2();
    int y2 = courseObject->y2();

    x1 = (x2 + x1) / 2;
    y1 = (y2 + y1) / 2;

    robotBack_ = std::make_unique<CourseObject>(x1, y1, x1, y1, "robotBack");
  }
  else if (name == "egg")
  {
    int x1 = courseObject->x1();
    int y1 = courseObject->y1();
    int x2 = courseObject->x2();
    int y2 = courseObject->y2();
    blockingObjects_.push_back(std::make_unique<Egg>(x1, y1, x2, y2));
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

void NavigationController::addBlockingObject(std::unique_ptr<VectorWithStartPos>&& blockingObject)
{
  blockingObjects_.push_back(std::move(blockingObject));
}

void NavigationController::clearObjects()
{
  ballVector_.clear();
  blockingObjects_.clear();
  safeSpots_.clear();
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
  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());

  if (frontIsToCloseToBlockingObject() && target_ == nullptr)
  {
    return std::make_unique<JourneyModel>(-10, 0, true);
  }

  if (target_ != nullptr)
  {
    /*
    if (not targetStillActual())
    {
      target_ = nullptr;
      return nullptr;
    }
    */
    auto vectorToObject = handleObjectNextToBlocking(target_.get());
    cv::arrowedLine(*MainController::getFrame(), {robotMiddle.x1(), robotMiddle.y1()},
                    {robotMiddle.x1() + vectorToObject.x, robotMiddle.y1() + vectorToObject.y},
                    cv::Scalar(255, 0, 255), 1,
                    cv::LINE_AA, 0, 0.01);
    auto directVectorToObject = MathUtil::calculateVectorToObject(robotFront_.get(), target_.get());
    if (directVectorToObject.getLength() < ConfigController::getConfigInt("DistanceBeforeTargetReached"))
    {
      Utility::appendToFile("log.txt", "target_ is now null\n");
      target_ = nullptr;
      sameTargetCount_ = 0;
      return nullptr;
    }

    if (checkCollisionOnRoute(vectorToObject))
    {
      vectorToObject = navigateToSafeSpot();
      if (vectorToObject.isNullVector())
      {
        std::cout << "could not find a safe, safe spot" << std::endl;
        return nullptr;
      }

      std::cout << "Navigating to safe spot: " << vectorToObject.x << " " << vectorToObject.y << std::endl;
    }

    return makeJourneyModel(vectorToObject, toCollectBalls_);
  }

  //MathUtil::correctCourseObjectForHeightOffset(robotBack_.get(), robotFront_.get());
  auto objectVector = Vector(0, 0);

  if (ballVector_.empty() || (ballVector_.size() == 5 && not hasDeliveredBallsOnce_))
  {
    goToGoalCount_++;
  }
  else
  {
    goToGoalCount_ = 0;
  }

  if (goToGoalCount_ >= stableThreshold)
  {
    objectVector = navigateToGoal();
    auto vectorToRobotBack = MathUtil::calculateVectorToObject(robotBack_.get(), robotFront_.get());
    auto goalVector = MathUtil::calculateVectorToObject(&robotMiddle, goal_.get());
    double angleDiff = MathUtil::calculateAngleDifferenceBetweenVectors(goalVector, vectorToRobotBack);
    cv::arrowedLine(*MainController::getFrame(), {robotMiddle.x1(), robotMiddle.y1()},
                    {robotMiddle.x1() + objectVector.x, robotMiddle.y1() + objectVector.y},
                    cv::Scalar(255, 0, 255), 1,
                    cv::LINE_AA, 0, 0.01);
    cv::putText(*MainController::getFrame(), "VA FANGOOL",
                {robotMiddle.x1() + objectVector.x, robotMiddle.y1() + objectVector.y + 30}, cv::FONT_HERSHEY_SIMPLEX,
                0.5, cv::Scalar(0, 255, 0), 2);
    if (objectVector.getLength() < ConfigController::getConfigInt("DistanceBeforeTargetReached"))
    {
      if (std::abs(angleDiff) > ConfigController::getConfigInt("AllowedAngleDifference"))
      {
        target_ = nullptr;
        return std::make_unique<JourneyModel>(0, -angleDiff, true);
      }
      target_ = nullptr;
      return std::make_unique<JourneyModel>(0, 0, false);
    }
    return makeJourneyModel(objectVector, true);
  }
  else
  {
    removeBallsInsideRobot();
    removeBallsOutsideCourse();
    objectVector = findClosestBall();
    toCollectBalls_ = true;
  }

  if (objectVector.isNullVector())
  {
    Utility::appendToFile("log.txt", "objectVector = {0,0}, firstCheck\n");
    return nullptr;
  }

  if (sameTargetCount_ == ConfigController::getConfigInt("AmountOfCommandsToAverage"))
  {
    target_ = std::move(potentialTarget_);
    potentialTarget_ = nullptr;
  }


  cv::arrowedLine(*MainController::getFrame(), {robotMiddle.x1(), robotMiddle.y1()},
                  {robotMiddle.x1() + objectVector.x, robotMiddle.y1() + objectVector.y},
                  cv::Scalar(255, 0, 255), 1,
                  cv::LINE_AA, 0, 0.01);
  cv::putText(*MainController::getFrame(), std::to_string(sameTargetCount_),
              {robotMiddle.x1() + objectVector.x, robotMiddle.y1() + objectVector.y + 30}, cv::FONT_HERSHEY_SIMPLEX,
              0.5, cv::Scalar(0, 255, 0), 2);

  return nullptr;
}

void NavigationController::setHasDeliveredOnce()
{
  hasDeliveredBallsOnce_ = true;
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
  int maxX = INT_MIN;
  int maxY = INT_MIN;

  for (const auto& blockingObject : blockingObjects_)
  {
    minX = std::min(minX, blockingObject->getLowestX());
    minY = std::min(minY, blockingObject->getLowestY());
    maxX = std::max(maxX, blockingObject->getMaxX());
    maxY = std::max(maxY, blockingObject->getMaxY());
  }

  auto deletionLambda = [minX,minY,maxX,maxY](const std::unique_ptr<CourseObject>& a) -> bool
  {
    return ((a->x1() < minX || a->x2() > maxX) || a->y1() < minY ||
      maxY < a->y2());
  };
  for (const auto& ball : ballVector_)
  {
    if (deletionLambda(ball))
    {
      cv::circle(*MainController::getFrame(), {(ball->x2() + ball->x1()) / 2, (ball->y2() + ball->y1()) / 2}, 10,
                 cv::Scalar(0, 0, 255), 5);
    }
  }

  //std::erase_if(ballVector_, deletionLambda);
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

Vector NavigationController::navigateToGoal()
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
  goal_ = std::make_unique<CourseObject>(goal.x, goal.y, goal.x, goal.y, "goal");
  auto localGoal = CourseObject(targetX, goal.y, targetX, goal.y, "goal");
  Utility::appendToFile(
    "log.txt",
    "Navigating to Goal: " + std::to_string(goal_->x1()) + ", " + std::to_string(goal_->y1()) + "\n");

  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());
  return MathUtil::calculateVectorToObject(&robotMiddle, &localGoal);
}

Vector NavigationController::findClosestBall()
{
  if (ballVector_.empty())
  {
    return {0, 0};
  }

  auto shortestVector = Vector(5000, 5000);
  CourseObject* closestBall = nullptr;
  for (const auto& ball : ballVector_)
  {
    auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());
    auto vectorToBall = MathUtil::calculateVectorToObject(&robotMiddle, ball.get());
    if (vectorToBall.getLength() < shortestVector.getLength() && not vectorToBall.isNullVector())
    {
      shortestVector = vectorToBall;
      closestBall = ball.get();
    }
  }
  if (closestBall != nullptr)
  {
    if (potentialTarget_ != nullptr)
    {
      if (potentialTarget_->courseObjectWithinValidRange(closestBall))
      {
        Utility::appendToFile("log.txt", "Same target count incremented to " + std::to_string(sameTargetCount_) + "\n");
        sameTargetCount_++;
      }
      else
      {
        Utility::appendToFile("log.txt", "New Potential Target (\"Ball\")\n");
        potentialTarget_ = std::make_unique<CourseObject>(closestBall->x1(), closestBall->y1(), closestBall->x2(),
                                                          closestBall->y2(),
                                                          "ball");
      }
    }
    else
    {
      Utility::appendToFile("log.txt", "New Potential Target (\"Ball\")\n");
      potentialTarget_ = std::make_unique<CourseObject>(closestBall->x1(), closestBall->y1(), closestBall->x2(),
                                                        closestBall->y2(),
                                                        "ball");
    }
    Utility::appendToFile(
      "log.txt",
      "Navigating to Ball: " + std::to_string(closestBall->x1()) + ", " + std::to_string(closestBall->y1()) + "\n");
  }
  else
  {
    Utility::appendToFile(
      "log.txt", "Navigating to Ball: BUT NO BALLS FOUND\n");
    return {0, 0};
  }
  return handleObjectNextToBlocking(closestBall);
}

Vector NavigationController::navigateToLeftGoal() const
{
  int minY = INT_MAX;
  int minX = INT_MAX;
  int maxY = INT_MIN;

  for (const auto& object : blockingObjects_)
  {
    const int x0 = object->startX();
    const int y0 = object->startY();

    const int x1 = x0 + object->x;
    const int y1 = y0 + object->y;

    minX = std::min({minX, x0, x1});
    minY = std::min({minY, y0, y1});
    maxY = std::max({maxY, y0, y1});
  }

  const int middleY = (minY + maxY) / 2;
  return Vector{minX, middleY};
}

Vector NavigationController::navigateToRightGoal() const
{
  int minY = INT_MAX;
  int maxX = INT_MIN;
  int maxY = INT_MIN;

  for (const auto& object : blockingObjects_)
  {
    const int x0 = object->startX();
    const int y0 = object->startY();

    const int x1 = x0 + object->x;
    const int y1 = y0 + object->y;

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

Vector NavigationController::handleObjectNextToBlocking(const CourseObject* courseObject) const
{
  if (courseObject == nullptr)
  {
    Utility::appendToFile("log.txt", "getVectorForObjectNearWall(), courseObject is nullptr");
    return {0, 0};
  }
  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());

  auto closestVectors = getVectorsForClosestBlockingObjects(courseObject);

  if (ConfigController::getConfigBool("showVectorsToWall"))
  {
    Vector startPoint = Vector((courseObject->x1() + courseObject->x2()) / 2,
                               (courseObject->y1() + courseObject->y2()) / 2);
    cv::arrowedLine(*MainController::getFrame(), {startPoint.x, startPoint.y},
                    {startPoint.x + closestVectors.first.x, startPoint.y + closestVectors.first.y},
                    cv::Scalar(0, 0, 255), 1,
                    cv::LINE_AA, 0, 0.01);
    cv::arrowedLine(*MainController::getFrame(), {startPoint.x, startPoint.y},
                    {startPoint.x + closestVectors.second.x, startPoint.y + closestVectors.second.y},
                    cv::Scalar(0, 0, 255), 1,
                    cv::LINE_AA, 0, 0.01);
  }

  if (closestVectors.second.getLength() > ConfigController::getConfigInt("DistanceToWallBeforeHandling"))
  {
    auto vectorToWall = closestVectors.first;

    if (vectorToWall.getLength() > ConfigController::getConfigInt("DistanceToWallBeforeHandling"))
    {
      return MathUtil::calculateVectorToObject(&robotMiddle, courseObject);
    }
    return handleObjectNearWall(courseObject, vectorToWall);
  }
  return handleObjectNearCorner(courseObject, closestVectors);
}

Vector NavigationController::handleObjectNearWall(const CourseObject* courseObject,
                                                  const Vector& vectorToWall) const
{
  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());
  // 1 wall
  auto localCourseObject = CourseObject(courseObject->x1(), courseObject->y1(), courseObject->x2(),
                                        courseObject->y2(), courseObject->name());
  int singleWallShiftDiff = ConfigController::getConfigInt("SingleWallShiftDiff");
  if (std::abs(vectorToWall.x) > std::abs(vectorToWall.y))
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

Vector NavigationController::handleObjectNearCorner(const CourseObject* courseObject,
                                                    const std::pair<Vector, Vector>&) const
{
  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());

  const int ballCenterX = (courseObject->x1() + courseObject->x2()) / 2;
  const int ballCenterY = (courseObject->y1() + courseObject->y2()) / 2;

  int imageWidth = 1920;
  int imageHeight = 1080;

  int shiftedX = (ballCenterX > imageWidth / 2) ? -1 : 1;
  int shiftedY = (ballCenterY > imageHeight / 2) ? -1 : 1;

  double angleDeg = 10;
  double angleRad = angleDeg * CV_PI / 180.0;
  int shiftDist = ConfigController::getConfigInt("DistanceToShiftedPointBeforeTurning") * 5;

  double dx = std::tan(angleRad) * shiftDist;

  CourseObject shiftedTarget(*courseObject);
  shiftedTarget.shiftX(shiftedX * dx);
  shiftedTarget.shiftY(shiftedY * shiftDist);

  auto vectorToIntermediaryPoint = MathUtil::calculateVectorToObject(&robotMiddle, &shiftedTarget);

  if (vectorToIntermediaryPoint.getLength() < ConfigController::getConfigInt("DistanceBeforeTargetReached"))
  {
    auto localObject = CourseObject(*courseObject);
    int shiftDistanceOnCornerBall = ConfigController::getConfigInt("ShiftDistanceOnCornerBall");
    if (courseObject->x1() > ConfigController::getConfigInt("middleXOnAxis"))
    {
      localObject.shiftX(-shiftDistanceOnCornerBall);
    }
    else
    {
      localObject.shiftX(shiftDistanceOnCornerBall);
    }
    return MathUtil::calculateVectorToObject(&robotMiddle, &localObject);
  }

  return vectorToIntermediaryPoint;
}

std::pair<Vector, Vector> NavigationController::getVectorsForClosestBlockingObjects(
  const CourseObject* courseObject) const
{
  auto returnPair = std::make_pair(Vector(5000, 5000), Vector(5000, 5000));
  for (const auto& blockingObject : blockingObjects_)
  {
    auto fromPointVector = Vector((courseObject->x1() + courseObject->x2()) / 2,
                                  (courseObject->y1() + courseObject->y2()) / 2);
    auto vector = blockingObject->closestVectorFromPoint(fromPointVector);
    /*cv::arrowedLine(*MainController::getFrame(), {fromPointVector.x, fromPointVector.y},
                    {fromPointVector.x + vector.x, fromPointVector.y + vector.y}, cv::Scalar(0, 0, 255), 1,
                    cv::LINE_AA, 0, 0.01);
*/
    if (returnPair.first.getLength() > vector.getLength())
    {
      returnPair.second = returnPair.first;
      returnPair.first = vector;
    }
    else if (returnPair.second.getLength() > vector.getLength())
    {
      returnPair.second = vector;
    }
  }
  return returnPair;
}

bool segmentsIntersect(int x1, int y1, int x2, int y2,
                       int x3, int y3, int x4, int y4)
{
  auto orientation = [](int ax, int ay, int bx, int by, int cx, int cy)
  {
    int val = (by - ay) * (cx - bx) - (bx - ax) * (cy - by);
    if (val == 0) return 0;
    return (val > 0) ? 1 : 2;
  };

  int o1 = orientation(x1, y1, x2, y2, x3, y3);
  int o2 = orientation(x1, y1, x2, y2, x4, y4);
  int o3 = orientation(x3, y3, x4, y4, x1, y1);
  int o4 = orientation(x3, y3, x4, y4, x2, y2);

  if (o1 != o2 && o3 != o4) return true;

  auto onSegment = [](int ax, int ay, int bx, int by, int cx, int cy)
  {
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
  if (!robotFront_ || !robotBack_)
  {
    return false;
  }
  const CourseObject robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());
  int startX = robotMiddle.x1();
  int startY = robotMiddle.y1();
  int endX = startX + targetVector.x;
  int endY = startY + targetVector.y;

  double length = std::sqrt(targetVector.x * targetVector.x + targetVector.y * targetVector.y);
  if (length == 0.0) return false;

  double halfWidth = robotWidth_ / 2.0;

  double perpX = -(targetVector.y / length);
  double perpY = (targetVector.x / length);

  double offsetX = perpX * halfWidth;
  double offsetY = perpY * halfWidth;

  struct Segment
  {
    double x1, y1, x2, y2;
  };

  Segment center = Segment(startX, startY, endX, endY);
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
    int bx1 = blocker->startX();
    int by1 = blocker->startY();
    int bx2 = bx1 + blocker->x;
    int by2 = by1 + blocker->y;

    if (segmentsIntersect(center.x1, center.y1, center.x2, center.y2, bx1, by1, bx2, by2) ||
      segmentsIntersect(left.x1, left.y1, left.x2, left.y2, bx1, by1, bx2, by2) ||
      segmentsIntersect(right.x1, right.y1, right.x2, right.y2, bx1, by1, bx2, by2))
    {
      return true;
    }
  }

  return false;
}

bool NavigationController::frontIsToCloseToBlockingObject() const
{
  auto vectorsToBlockingObjects = getVectorsForClosestBlockingObjects(robotFront_.get());
  if (vectorsToBlockingObjects.first.getLength() < ConfigController::getConfigInt("DistanceBeforeToCloseToWall"))
  {
    return true;
  }
  return false;
}

bool NavigationController::targetStillActual()
{
  for (const auto& ball : ballVector_)
  {
    int x1Diff = std::abs(ball->x1() - target_->x1());
    int x2Diff = std::abs(ball->x2() - target_->x2());
    int y1Diff = std::abs(ball->y1() - target_->y1());
    int y2Diff = std::abs(ball->y2() - target_->y2());

    int allowedDiff = ConfigController::getConfigInt("AllowedDifferenceBetweenNewTargetAndPotential");
    if (x1Diff < allowedDiff && y1Diff < allowedDiff && x2Diff < allowedDiff && y2Diff < allowedDiff)
    {
      return true;
    }
  }
  return false;
}

Vector NavigationController::navigateToSafeSpot()
{
  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());
  for (const auto& safeSpot : safeSpots_)
  {
    CourseObject courseObject(safeSpot.first, safeSpot.second, safeSpot.first, safeSpot.second, "");
    Vector vectorToObject = MathUtil::calculateVectorToObject(&robotMiddle, &courseObject);
    if (!checkCollisionOnRoute(vectorToObject))
    {
      return {safeSpot.first, safeSpot.second};
    }
  }

  return {0, 0};
}

void NavigationController::findSafeSpots()
{
  int minY = INT_MAX;
  int minX = INT_MAX;
  int maxY = INT_MIN;
  int maxX = INT_MIN;

  for (const auto& object : blockingObjects_)
  {
    const int x0 = object->startX();
    const int y0 = object->startY();

    const int x1 = x0 + object->x;
    const int y1 = y0 + object->y;

    minX = std::min({minX, x0, x1});
    minY = std::min({minY, y0, y1});
    maxY = std::max({maxY, y0, y1});
    maxX = std::max({maxX, x0, x1});
  }

  const int xOffset = (maxX - minX) / 3;
  const int yOffset = (maxY - minY) / 3;

  for (int i = 1; i < 2; i++)
  {
    for (int j = 1; j < 2; j++)
    {
      safeSpots_.emplace_back(minX + i * xOffset, minY + j * yOffset);
    }
  }
}

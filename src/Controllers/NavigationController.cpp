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
#include "../Models/VectorToBlockingObject.h"

void NavigationController::addCourseObject(std::unique_ptr<CourseObject> &&courseObject)
{
  const std::string name = courseObject->name();
  if (name == "white" || name == "orange")
  {
    ballVector_.push_back(std::move(courseObject));
  } else if (name == "robotFront")
  {
    int x1 = courseObject->x1();
    int y1 = courseObject->y1();
    int x2 = courseObject->x2();
    int y2 = courseObject->y2();

    x1 = (x2 + x1) / 2;
    y1 = (y2 + y1) / 2;

    robotFront_ = std::make_unique<CourseObject>(x1, y1, x1, y1, "robotFront");
  } else if (name == "robotBack")
  {
    int x1 = courseObject->x1();
    int y1 = courseObject->y1();
    int x2 = courseObject->x2();
    int y2 = courseObject->y2();

    x1 = (x2 + x1) / 2;
    y1 = (y2 + y1) / 2;

    robotBack_ = std::make_unique<CourseObject>(x1, y1, x1, y1, "robotBack");
  } else if (name == "egg")
  {
    int x1 = courseObject->x1();
    int y1 = courseObject->y1();
    int x2 = courseObject->x2();
    int y2 = courseObject->y2();
    wallObjects_.push_back(std::make_unique<Egg>(x1, y1, x2, y2));
  } else if (name == "goal")
  {
    if (goal_ == nullptr)
    {
      goal_ = std::move(courseObject);
    } else
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
  } else
  {
    throw std::runtime_error("Invalid courseObject name" + name);
  }
}

void NavigationController::addBlockingObject(std::unique_ptr<VectorWithStartPos> &&blockingObject)
{
  amountOfWalls_++;
  wallObjects_.push_back(std::move(blockingObject));
}

void NavigationController::addCrossObject(std::unique_ptr<VectorWithStartPos> &&blockingObject)
{
  crossObjects_.push_back(std::move(blockingObject));
}

void NavigationController::addGoalObject(std::unique_ptr<CourseObject> &&goalObject)
{
  goal_ = std::move(goalObject);
}

void NavigationController::clearObjects()
{
  ballVector_.clear();
  wallObjects_.clear();
  crossObjects_.clear();
  safeSpots_.clear();
  goal_ = nullptr;
  robotFront_ = nullptr;
  robotBack_ = nullptr;
  amountOfWalls_ = 0;
}

std::unique_ptr<JourneyModel> NavigationController::calculateDegreesAndDistanceToObject()
{
  if (robotFront_ == nullptr || robotBack_ == nullptr)
  {
    Utility::appendToFile("log.txt", "No Robot\n");
    return nullptr;
  }
  if (target_ != nullptr)
  {
    auto now = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::seconds>(now - targetTimeStamp_);
    if (totalDuration.count() > 15)
    {
      target_ = nullptr;
    }
  }
  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());
  if (not lastSentCommandWasCompleted_)
  {
    Utility::appendToFile("log.txt", "Waiting for command Completion\n");
    return nullptr;
  }

  MathUtil::correctCourseObjectForPerspective(robotBack_.get(), robotFront_.get());
  auto objectVector = Vector(0, 0);
  findSafeSpots();


  if (ballVector_.empty() || (ballVector_.size() <= 5 && not hasDeliveredBallsOnce_))
  {
    goToGoalCount_++;
  }
  else
  {
    goToGoalCount_ = 0;
  }
  if (atGoal_ && ((hasDeliveredBallsOnce_ && (not ballVector_.empty())) || sentShootAt0Balls_))
  {
    auto now = std::chrono::high_resolution_clock::now();
    if (now - atGoalTime_ > std::chrono::milliseconds(ConfigController::getConfigInt("GoalSleepInMilli")))
    {
      atGoal_ = false;
      navigatedToGoalIntermediate_ = false;
      return std::make_unique<JourneyModel>(-10, 0, true);
    }
    return nullptr;
  }
  if (ballVector_.size() > 6 && hasDeliveredBallsOnce_)
  {
    hasDeliveredBallsOnce_ = false;
  }

  if (goToGoalCount_ >= stableThreshold || forceGoToGoal_)
  {
    if (distanceToBackUp > 0)
    {
      auto journey = std::make_unique<JourneyModel>(-distanceToBackUp, 0, true);
      distanceToBackUp = 0;
      return std::move(journey);
    }
    objectVector = navigateToGoal(&robotMiddle);
    auto vectorToRobotBack = MathUtil::calculateVectorToObject(robotBack_.get(), robotFront_.get());
    auto goalVector = MathUtil::calculateVectorToObject(&robotMiddle, goal_.get());
    auto shootingCourseObject = CourseObject(*goal_);
    int shootingDistance = ConfigController::getConfigInt("GoalShootingDistance");
    bool goalIsLeft = goal_->x1() > ConfigController::getConfigInt("middleXOnAxis");

    int shiftDistance = goalIsLeft ? -shootingDistance : shootingDistance;

    shootingCourseObject.shiftX(shiftDistance);
    auto shootingVector = MathUtil::calculateVectorToObject(&robotMiddle, &shootingCourseObject);
    double angleDiff = MathUtil::calculateAngleDifferenceBetweenVectors(goalVector, vectorToRobotBack);

    if (objectVector.getLength() < ConfigController::getConfigInt("DistanceBeforeTargetReached") ||
        navigatedToGoalIntermediate_)
    {
      navigatedToGoalIntermediate_ = true;
      if (shootingVector.getLength() < ConfigController::getConfigInt("DistanceBeforeTargetReached"))
      {
        if (std::abs(angleDiff) > ConfigController::getConfigInt("AllowedAngleDifference"))
        {
          cv::arrowedLine(*MainController::getFrame(), {robotMiddle.x1(), robotMiddle.y1()},
                          {robotMiddle.x1() + goalVector.x, robotMiddle.y1() + goalVector.y},
                          cv::Scalar(255, 0, 255), 1,
                          cv::LINE_AA, 0, 0.01);
          cv::putText(*MainController::getFrame(), "Timmy Turner: " + std::to_string(-angleDiff),
                      {robotMiddle.x1() + goalVector.x, robotMiddle.y1() + goalVector.y + 30}, cv::FONT_HERSHEY_SIMPLEX,
                      0.5, cv::Scalar(0, 255, 0), 2);
          return std::make_unique<JourneyModel>(0, -angleDiff, true);
        }

        atGoalTime_ = std::chrono::high_resolution_clock::now();
        atGoal_ = true;
        target_ = nullptr;
        Utility::appendToFile("log.txt", "Shooting with angle: " + std::to_string(angleDiff) + "\n");
        return std::make_unique<JourneyModel>(0, 0, false);
      }
      cv::arrowedLine(*MainController::getFrame(), {robotMiddle.x1(), robotMiddle.y1()},
                      {robotMiddle.x1() + shootingVector.x, robotMiddle.y1() + shootingVector.y},
                      cv::Scalar(255, 0, 255), 1,
                      cv::LINE_AA, 0, 0.01);
      cv::putText(*MainController::getFrame(), "VA FANGOOL",
                  {robotMiddle.x1() + shootingVector.x, robotMiddle.y1() + shootingVector.y + 30},
                  cv::FONT_HERSHEY_SIMPLEX,
                  0.5, cv::Scalar(0, 255, 0), 2);
      goingToIntermediate_ = true;
      return makeJourneyModel(shootingVector, true);
    }

    if (checkCollisionOnRoute(objectVector))
    {
      objectVector = navigateToSafeSpot(true);
      if (objectVector.isNullVector())
      {
        std::cout << "could not find a safe, safe spot" << std::endl;
        return nullptr;
      }

      std::cout << "Navigating to safe spot: " << objectVector.x << " " << objectVector.y << std::endl;
    }
    cv::arrowedLine(*MainController::getFrame(), {robotMiddle.x1(), robotMiddle.y1()},
                    {robotMiddle.x1() + objectVector.x, robotMiddle.y1() + objectVector.y},
                    cv::Scalar(255, 0, 255), 1,
                    cv::LINE_AA, 0, 0.01);
    cv::putText(*MainController::getFrame(), "VA FANGOOL",
                {robotMiddle.x1() + objectVector.x, robotMiddle.y1() + objectVector.y + 30}, cv::FONT_HERSHEY_SIMPLEX,
                0.5, cv::Scalar(0, 255, 0), 2);
    goingToIntermediate_ = true;
    return makeJourneyModel(objectVector, true);
  }

  if (target_ != nullptr)
  {
    auto vectorToObject = handleObjectNextToBlocking(target_.get());
    cv::arrowedLine(*MainController::getFrame(), {robotMiddle.x1(), robotMiddle.y1()},
                    {robotMiddle.x1() + vectorToObject.x, robotMiddle.y1() + vectorToObject.y},
                    cv::Scalar(255, 0, 255), 1,
                    cv::LINE_AA, 0, 0.01);
    auto directVectorToObject = MathUtil::calculateVectorToObject(robotFront_.get(), target_.get());
    if (directVectorToObject.getLength() < ConfigController::getConfigInt("DistanceBeforeTargetReached"))
    {
      Utility::appendToFile("log.txt", "target_ is now null\n");
      auto closestVector = getVectorsForClosestBlockingObjects(target_.get()).first;
      target_ = nullptr;
      sameTargetCount_ = 0;
      if (distanceToBackUp > 0)
      {
        auto journey = std::make_unique<JourneyModel>(-distanceToBackUp, 0, true);
        distanceToBackUp = 0;
        return std::move(journey);
      }
      return nullptr;
    }

    if (checkCollisionOnRoute(vectorToObject))
    {
      vectorToObject = navigateToSafeSpot(false);
      if (vectorToObject.isNullVector())
      {
        std::cout << "could not find a safe, safe spot" << std::endl;
        return nullptr;
      }

      std::cout << "Navigating to safe spot: " << vectorToObject.x << " " << vectorToObject.y << std::endl;
    }

    return makeJourneyModel(vectorToObject, toCollectBalls_);
  }

  removeBallsInsideRobot();
  removeBallsOutsideCourse();
  objectVector = findClosestBall(&robotMiddle);
  toCollectBalls_ = true;

  if (objectVector.isNullVector())
  {
    Utility::appendToFile("log.txt", "objectVector = {0,0}, firstCheck\n");
    return nullptr;
  }

  if (sameTargetCount_ == ConfigController::getConfigInt("TargetSameBeforeTargetSet"))
  {
    target_ = std::move(potentialTarget_);
    targetTimeStamp_ = std::chrono::high_resolution_clock::now();
    potentialTarget_ = nullptr;
    navigatedToGoalIntermediate_ = false;
    auto closestVectors = getVectorsForClosestBlockingObjects(target_.get());
    double distanceToWall = closestVectors.first.vector.getLength();
    if (distanceToWall < ConfigController::getConfigInt("DistanceBeforeToCloseToWall"))
    {
      if (closestVectors.second.vector.getLength() < ConfigController::getConfigInt("DistanceBeforeToCloseToWall"))
      {
        distanceToBackUp = 30;
      } else
      {
        distanceToBackUp = 10;
      }
    }
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
  if (ballVector_.empty())
  {
    sentShootAt0Balls_ = true;
  }
}

void NavigationController::lastSentCommandWasCompleted()
{
  lastSentCommandWasCompleted_ = true;
}

void NavigationController::newCommandSent()
{
  lastSentCommandWasCompleted_ = false;
}

std::unique_ptr<JourneyModel> NavigationController::makeJourneyModel(const Vector &objectVector,
                                                                     bool toCollectBalls)
{
  const Vector robotVector = {robotFront_->x1() - robotBack_->x1(), robotFront_->y1() - robotBack_->y1()};
  const double angle = MathUtil::calculateAngleDifferenceBetweenVectors(robotVector, objectVector);

  auto vectorToRobotBack = MathUtil::calculateVectorToObject(robotFront_.get(), robotBack_.get());
  double distanceInCm = objectVector.getLength() * ((static_cast<double>(ConfigController::getConfigInt(
                                                       "RobotLengthInMM")) /
                                                     10) / vectorToRobotBack.getLength());

  bool localCross = ballNearCross_;
  bool localIntermediate = goingToIntermediate_;
  bool localSafeSpot = goingToSafeSpot_;
  ballNearCross_ = false;
  goingToIntermediate_ = false;
  goingToSafeSpot_ = false;
  return std::make_unique<JourneyModel>(distanceInCm, angle, toCollectBalls, localCross,localIntermediate, localSafeSpot);
}

void NavigationController::removeBallsOutsideCourse()
{
  if (wallObjects_.empty())
  {
    return;
  }

  int minX = INT_MAX;
  int minY = INT_MAX;
  int maxX = INT_MIN;
  int maxY = INT_MIN;

  for (const auto &blockingObject: wallObjects_)
  {
    minX = std::min(minX, blockingObject->getLowestX());
    minY = std::min(minY, blockingObject->getLowestY());
    maxX = std::max(maxX, blockingObject->getMaxX());
    maxY = std::max(maxY, blockingObject->getMaxY());
  }

  auto deletionLambda = [minX,minY,maxX,maxY](const std::unique_ptr<CourseObject> &a) -> bool {
    int middleX = (a->x1() + a->x2()) / 2;
    int middleY = (a->y1() + a->y2()) / 2;

    return not ((middleX > minX && middleX < maxX) && middleY > minY &&
            maxY > middleY);
  };
  for (const auto &ball: ballVector_)
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

  std::erase_if(ballVector_, [topX, topY, bottomX, bottomY](const std::unique_ptr<CourseObject> &ball) {
    double ballTopX = std::min(ball->x1(), ball->x2());
    double ballTopY = std::min(ball->y1(), ball->y2());
    double ballBottomX = std::max(ball->x1(), ball->x2());
    double ballBottomY = std::max(ball->y1(), ball->y2());

    return ballBottomX > topX && ballTopX < bottomX && ballBottomY > topY && ballTopY < bottomY;
  });
}

Vector NavigationController::navigateToGoal(CourseObject *fromObject)
{
  int targetX;

  if (goal_->x1() > ConfigController::getConfigInt("middleXOnAxis"))
  {
    targetX = goal_->x1() - ConfigController::getConfigInt("GoalIntermediatePointDistance");
  } else
  {
    targetX = goal_->x1() + ConfigController::getConfigInt("GoalIntermediatePointDistance");
  }
  auto localGoal = CourseObject(targetX, goal_->y1(), targetX, goal_->y1(), "goal");
  Utility::appendToFile(
    "log.txt",
    "Navigating to Goal: " + std::to_string(goal_->x1()) + ", " + std::to_string(goal_->y1()) + "\n");

  return MathUtil::calculateVectorToObject(fromObject, &localGoal);
}

Vector NavigationController::findClosestBall(CourseObject *fromObject)
{
  if (ballVector_.empty())
  {
    return {0, 0};
  }

  auto shortestVector = Vector(5000, 5000);
  CourseObject *closestBall = nullptr;
  auto deliverOrangeFirst = ConfigController::getConfigBool("DeliverOrangeFirst");
  bool orangeOnCourse = false;

  for (const auto &ball: ballVector_) {
    if (ball->name() == "orange") {
      orangeOnCourse = true;
    }
  }
  for (const auto &ball: ballVector_)
  {
    auto vectorToBall = MathUtil::calculateVectorToObject(fromObject, ball.get());

    if (ballVector_.size() == 6 && not hasDeliveredBallsOnce_ && deliverOrangeFirst && orangeOnCourse)
    {
      if (ball->name() != "orange")
      {
        continue;
      }

      closestBall = ball.get();
      break;
    }

    if (vectorToBall.getLength() < shortestVector.getLength() && not vectorToBall.isNullVector())
    {
      if (ballVector_.size() > 6 && ball->name() == "orange" && deliverOrangeFirst)
      {
        continue;
      }

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
      } else
      {
        Utility::appendToFile("log.txt", "New Potential Target (\"Ball\")\n");
        potentialTarget_ = std::make_unique<CourseObject>(closestBall->x1(), closestBall->y1(), closestBall->x2(),
                                                          closestBall->y2(),
                                                          "ball");
      }
    } else
    {
      Utility::appendToFile("log.txt", "New Potential Target (\"Ball\")\n");
      potentialTarget_ = std::make_unique<CourseObject>(closestBall->x1(), closestBall->y1(), closestBall->x2(),
                                                        closestBall->y2(),
                                                        "ball");
    }
    Utility::appendToFile(
      "log.txt",
      "Navigating to Ball: " + std::to_string(closestBall->x1()) + ", " + std::to_string(closestBall->y1()) + "\n");
  } else
  {
    Utility::appendToFile(
      "log.txt", "Navigating to Ball: BUT NO BALLS FOUND\n");
    return {0, 0};
  }
  return handleObjectNextToBlocking(closestBall);
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
    } else if (currentY_ == ConfigController::getConfigInt("safeYTop") && currentX_ ==
               ConfigController::getConfigInt("safeXRight"))
    {
      currentY_++;
    } else if (currentY_ == ConfigController::getConfigInt("safeYTop"))
    {
      currentX_++;
    } else if (currentX_ == ConfigController::getConfigInt("safeXRight") && currentY_ ==
               ConfigController::getConfigInt("safeYBot"))
    {
      currentX_--;
    } else if (currentX_ == ConfigController::getConfigInt("safeXRight"))
    {
      currentY_++;
    } else if (currentY_ == ConfigController::getConfigInt("safeYBot"))
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

Vector NavigationController::handleObjectNextToBlocking(const CourseObject *courseObject)
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
                    {startPoint.x + closestVectors.first.vector.x, startPoint.y + closestVectors.first.vector.y},
                    cv::Scalar(0, 0, 255), 1,
                    cv::LINE_AA, 0, 0.01);
    cv::arrowedLine(*MainController::getFrame(), {startPoint.x, startPoint.y},
                    {startPoint.x + closestVectors.second.vector.x, startPoint.y + closestVectors.second.vector.y},
                    cv::Scalar(0, 0, 255), 1,
                    cv::LINE_AA, 0, 0.01);
  }

  double closestVectorsAngleDiff = MathUtil::calculateAngleDifferenceBetweenVectors(
    closestVectors.first.vector, closestVectors.second.vector);
  int maxAllowedAngleDiffBetweenClosestVectors = ConfigController::getConfigInt("AngleDiffBeforeCornerBall");
  if ((closestVectors.second.vector.getLength() > ConfigController::getConfigInt("DistanceToWallBeforeHandling")
       || std::abs(closestVectorsAngleDiff) < maxAllowedAngleDiffBetweenClosestVectors))
  {
    auto vectorToWall = closestVectors.first;

    if (vectorToWall.vector.getLength() > ConfigController::getConfigInt("DistanceToWallBeforeHandling"))
    {
      return MathUtil::calculateVectorToObject(&robotMiddle, courseObject);
    }
    return handleObjectNearWall(courseObject, vectorToWall.vector);
  }

  if (closestVectors.first.vector.getLength() > ConfigController::getConfigInt("DistanceToWallBeforeHandling"))
  {
    return MathUtil::calculateVectorToObject(&robotMiddle, courseObject);
  }

  auto vectorPair = std::make_pair(closestVectors.first.vector, closestVectors.second.vector);
  if (closestVectors.first.isCross)
  {
    return handleObjectNearCross(courseObject, vectorPair);
  }
  return handleObjectNearCorner(courseObject, vectorPair);
}

Vector NavigationController::handleObjectNearWall(const CourseObject *courseObject,
                                                  const Vector &vectorToWall)
{
  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());
  // 1 wall
  auto localCourseObject = CourseObject(courseObject->x1(), courseObject->y1(), courseObject->x2(),
                                        courseObject->y2(), courseObject->name());
  int singleWallShiftDiff = ConfigController::getConfigInt("SingleWallShiftDiff");
  if (std::abs(vectorToWall.x) > std::abs(vectorToWall.y))
  {
    localCourseObject.shiftX(vectorToWall.x > 0 ? -singleWallShiftDiff : singleWallShiftDiff);
  } else
  {
    localCourseObject.shiftY(vectorToWall.y > 0 ? -singleWallShiftDiff : singleWallShiftDiff);
  }


  auto vectorToDiffPoint = MathUtil::calculateVectorToObject(&robotMiddle, &localCourseObject);
  if (vectorToDiffPoint.getLength() < ConfigController::getConfigInt("DistanceToShiftedPointBeforeTurning"))
  {
    return MathUtil::calculateVectorToObject(&robotMiddle, courseObject);
  }
  goingToIntermediate_ = true;
  return MathUtil::calculateVectorToObject(&robotMiddle, &localCourseObject);
}

Vector NavigationController::handleObjectNearCorner(const CourseObject *courseObject,
                                                    const std::pair<Vector, Vector> &)
{
  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());

  const int ballCenterX = (courseObject->x1() + courseObject->x2()) / 2;
  const int ballCenterY = (courseObject->y1() + courseObject->y2()) / 2;

  int imageWidth = 1920;
  int imageHeight = 1080;

  int shiftedX = (ballCenterX > imageWidth / 2) ? -1 : 1;
  int shiftedY = (ballCenterY > imageHeight / 2) ? -1 : 1;

  int angleDeg = ConfigController::getConfigInt("CornerApproachAngle");
  double angleRad = angleDeg * CV_PI / 180.0;
  int shiftDist = ConfigController::getConfigInt("DistanceToShiftedPointBeforeTurning") * 6;

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
    } else
    {
      localObject.shiftX(shiftDistanceOnCornerBall);
    }
    return MathUtil::calculateVectorToObject(&robotMiddle, &localObject) * 1.4;
  }
  goingToIntermediate_ = true;
  return vectorToIntermediaryPoint;
}

Vector NavigationController::handleObjectNearCross(const CourseObject *courseObject,
                                                   const std::pair<Vector,Vector> &vectors)
{
  CourseObject robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());

  if (vectors.second.getLength() > ConfigController::getConfigInt("DistanceToWallBeforeHandling")*2)
  {
    return MathUtil::calculateVectorToObject(&robotMiddle, courseObject);
  }
  Vector shiftedVector = Vector(vectors.second);
  shiftedVector = Vector(-shiftedVector.x,-shiftedVector.y);
  shiftedVector.x = 1.0 / shiftedVector.getLength() * shiftedVector.x * (robotWidth_/4);
  shiftedVector.y = 1.0/ shiftedVector.getLength() * shiftedVector.y * (robotWidth_/4);

  const int ballX = (courseObject->x1() + courseObject->x2()) / 2 + shiftedVector.x;
  const int ballY = (courseObject->y1() + courseObject->y2()) / 2 + shiftedVector.y;
  Vector ballCentre(ballX, ballY);
  auto closestCrossVector = VectorWithStartPos(ballX, ballY, vectors.first);
  Vector offsetVector = vectors.first * ((1.0/vectors.first.getLength()) * 200);
  auto intermediatePoint = Vector(ballX + -offsetVector.x, ballY + -offsetVector.y);
  auto intermediateCourseObject = CourseObject(intermediatePoint.x,intermediatePoint.y,intermediatePoint.x,intermediatePoint.y,"");

  auto vectorToIntermediatePoint = MathUtil::calculateVectorToObject(&robotMiddle,&intermediateCourseObject);
  if (vectorToIntermediatePoint.getLength() < ConfigController::getConfigInt("DistanceToShiftedPointBeforeTurning"))
  {
    ballNearCross_ = true;
    auto localCourseObjcet = CourseObject(*courseObject);
    localCourseObjcet.shiftX(shiftedVector.x);
    localCourseObjcet.shiftY(shiftedVector.y);
    return MathUtil::calculateVectorToObject(&robotMiddle, &localCourseObjcet);
  }
  cv::circle(*MainController::getFrame(),{intermediatePoint.x,intermediatePoint.y},25,cv::Scalar(0,0,255));
  goingToIntermediate_ = true;
  return vectorToIntermediatePoint;
}


bool NavigationController::checkCollisionOnRoute(const Vector &targetVector) const
{
  if (!robotFront_ || !robotBack_ || crossObjects_.size() != 2)
  {
    return true;
  }
  std::vector<CourseObject> robotMiddles;
  robotMiddles.reserve(3);
  Vector acrossRobotVector = Vector(targetVector.y, -targetVector.x);
  int robotWidth = ConfigController::getConfigInt("RobotWidth");
  acrossRobotVector = acrossRobotVector * ((static_cast<double>(robotWidth) / 2) / acrossRobotVector.getLength());

  auto robotMiddleObject = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());
  auto robotLeftSide = CourseObject(robotMiddleObject);
  robotMiddles.emplace_back(robotMiddleObject);
  robotLeftSide.shiftX(acrossRobotVector.x);
  robotLeftSide.shiftY(acrossRobotVector.y);
  robotMiddles.push_back(robotLeftSide);

  auto robotRightSide = CourseObject(robotMiddleObject);
  robotRightSide.shiftX(-acrossRobotVector.x);
  robotRightSide.shiftY(-acrossRobotVector.y);
  robotMiddles.push_back(robotRightSide);

  int behindCount = 0;
  bool collisionDetected = false;
  for (const auto &robotMiddle: robotMiddles)
  {
    auto backToFrontVector = MathUtil::calculateVectorToObject(robotBack_.get(), robotFront_.get());
    auto cross1 = crossObjects_[0].get();
    auto cross2 = crossObjects_[1].get();

    auto point1 = CourseObject(cross1->startX_, cross1->startY_, cross1->startX_, cross1->startY_, "");
    auto point2 = CourseObject(cross1->startX_ + cross1->x, cross1->startY_ + cross1->y, cross1->startX_ + cross1->x,
                               cross1->startY_ + cross1->y, "");
    auto point3 = CourseObject(cross2->startX_, cross2->startY_, cross2->startX_, cross2->startY_, "");
    auto point4 = CourseObject(cross2->startX_ + cross2->x, cross2->startY_ + cross2->y, cross2->startX_ + cross2->x,
                               cross2->startY_ + cross2->y, "");
    int middleX = (cross1->startX_ + cross1->startX_ + cross1->x) / 2;
    int middleY = (cross1->startY_ + cross1->startY_) / 2;
    auto middlePoint = CourseObject(middleX, middleY, middleX, middleY, "");
    /*
        cv::arrowedLine(*MainController::getFrame(), {robotMiddle.x1(), robotMiddle.y1()},
                        {point1.x1(), point1.y1()},
                        cv::Scalar(255, 0, 255), 1,
                        cv::LINE_AA, 0, 0.01);
        cv::arrowedLine(*MainController::getFrame(), {robotMiddle.x1(), robotMiddle.y1()},
                        {point2.x1(), point2.y1()},
                        cv::Scalar(255, 0, 255), 1,
                        cv::LINE_AA, 0, 0.01);
        cv::arrowedLine(*MainController::getFrame(), {robotMiddle.x1(), robotMiddle.y1()},
                        {point3.x1(), point3.y1()},
                        cv::Scalar(255, 0, 255), 1,
                        cv::LINE_AA, 0, 0.01);
        cv::arrowedLine(*MainController::getFrame(), {robotMiddle.x1(), robotMiddle.y1()},
                        {point4.x1(), point4.y1()},
                        cv::Scalar(255, 0, 255), 1,
                        cv::LINE_AA, 0, 0.01);
    */
    auto point1Vector = MathUtil::calculateVectorToObject(&robotMiddle, &point1);
    auto point2Vector = MathUtil::calculateVectorToObject(&robotMiddle, &point2);
    auto point3Vector = MathUtil::calculateVectorToObject(&robotMiddle, &point3);
    auto point4Vector = MathUtil::calculateVectorToObject(&robotMiddle, &point4);
    auto middleVector = MathUtil::calculateVectorToObject(&robotMiddle, &middlePoint);

    auto point1Angle = MathUtil::calculateAngleDifferenceBetweenVectors(backToFrontVector, point1Vector);
    auto point2Angle = MathUtil::calculateAngleDifferenceBetweenVectors(backToFrontVector, point2Vector);
    auto point3Angle = MathUtil::calculateAngleDifferenceBetweenVectors(backToFrontVector, point3Vector);
    auto point4Angle = MathUtil::calculateAngleDifferenceBetweenVectors(backToFrontVector, point4Vector);
    auto middleAngle = MathUtil::calculateAngleDifferenceBetweenVectors(backToFrontVector, middleVector);

    double minAngle = std::min(std::min(point1Angle, point2Angle), std::min(point3Angle, point4Angle));
    double maxAngle = std::max(std::max(point1Angle, point2Angle), std::max(point3Angle, point4Angle));

    auto direction = MathUtil::calculateAngleDifferenceBetweenVectors(targetVector, middleVector);

    if (direction > 90 || direction < -90)
    {
      behindCount++;
      if (behindCount == robotMiddles.size())
      {
        return false;
      }
    }

    double angleToTargetVector = MathUtil::calculateAngleDifferenceBetweenVectors(backToFrontVector, targetVector);
    if (not(angleToTargetVector < minAngle || angleToTargetVector > maxAngle) && middleVector.getLength() < targetVector
        .getLength())
    {
      cv::arrowedLine(*MainController::getFrame(), {robotMiddle.x1(), robotMiddle.y1()},
                      {robotMiddle.x1() + targetVector.x, robotMiddle.y1() + targetVector.y}, cv::Scalar(255, 0, 255),
                      1);
      collisionDetected = true;
    }
  }
  return collisionDetected;
}

std::pair<VectorToBlockingObject, VectorToBlockingObject> NavigationController::getVectorsForClosestBlockingObjects(
  const CourseObject *courseObject
)
const
{
  const auto startVector = Vector(5000, 5000);
  auto returnPair = std::make_pair(VectorToBlockingObject(startVector, false),
                                   VectorToBlockingObject(startVector, false));

  for (const auto &blockingObject: crossObjects_)
  {
    auto fromPointVector = Vector((courseObject->x1() + courseObject->x2()) / 2,
                                  (courseObject->y1() + courseObject->y2()) / 2);
    auto vector = blockingObject->closestVectorFromPoint(fromPointVector);
    /*cv::arrowedLine(*MainController::getFrame(), {fromPointVector.x, fromPointVector.y},
                    {fromPointVector.x + vector.x, fromPointVector.y + vector.y}, cv::Scalar(0, 0, 255), 1,
                    cv::LINE_AA, 0, 0.01);
    */
    if (returnPair.first.vector.getLength() > vector.getLength())
    {
      returnPair.second = returnPair.first;
      returnPair.first.vector = vector;
      returnPair.first.isCross = true;
    } else if (returnPair.second.vector.getLength() > vector.getLength())
    {
      returnPair.second.vector = vector;
      returnPair.first.isCross = true;
    }
  }

  for (const auto &blockingObject: wallObjects_)
  {
    auto fromPointVector = Vector((courseObject->x1() + courseObject->x2()) / 2,
                                  (courseObject->y1() + courseObject->y2()) / 2);
    auto vector = blockingObject->closestVectorFromPoint(fromPointVector);
    /*cv::arrowedLine(*MainController::getFrame(), {fromPointVector.x, fromPointVector.y},
                    {fromPointVector.x + vector.x, fromPointVector.y + vector.y}, cv::Scalar(0, 0, 255), 1,
                    cv::LINE_AA, 0, 0.01);
    */
    if (returnPair.first.vector.getLength() > vector.getLength())
    {
      returnPair.second = returnPair.first;
      returnPair.first.vector = vector;
      returnPair.first.isCross = false;
    } else if (returnPair.second.vector.getLength() > vector.getLength())
    {
      returnPair.second.vector = vector;
      returnPair.first.isCross = false;
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

Vector NavigationController::navigateToSafeSpot(bool toGoal)
{
  auto robotMiddle = MathUtil::getRobotMiddle(robotBack_.get(), robotFront_.get());
  auto safeSpot = safeSpots_[currentSafeSpotIndex_];
  int startIndex = currentSafeSpotIndex_;
  int allowedDistance = ConfigController::getConfigInt("DistanceBeforeTargetReached");
  CourseObject courseObject(safeSpot.first, safeSpot.second, safeSpot.first, safeSpot.second, "safeSpot");
  Vector vectorToObject = MathUtil::calculateVectorToObject(&robotMiddle, &courseObject);
  if (vectorToObject.getLength() < allowedDistance)
  {
    safeSpot = safeSpots_[currentSafeSpotIndex_];
    courseObject = CourseObject(safeSpot.first, safeSpot.second, safeSpot.first, safeSpot.second, "safeSpot");
    vectorToObject = MathUtil::calculateVectorToObject(&robotMiddle, &courseObject);
    currentSafeSpotIndex_++;
    currentSafeSpotIndex_ %= 4;
  }
  while (checkCollisionOnRoute(vectorToObject))
  {
    safeSpot = safeSpots_[currentSafeSpotIndex_];
    courseObject = CourseObject(safeSpot.first, safeSpot.second, safeSpot.first, safeSpot.second, "safeSpot");
    vectorToObject = MathUtil::calculateVectorToObject(&robotMiddle, &courseObject);
    currentSafeSpotIndex_++;
    currentSafeSpotIndex_ %= 4;
    if (startIndex == currentSafeSpotIndex_)
    {
      return {0,0};
    }
  }
  goingToSafeSpot_ = true;
  return vectorToObject;
}

void NavigationController::findSafeSpots()
{
  int minY = INT_MAX;
  int minX = INT_MAX;
  int maxY = INT_MIN;
  int maxX = INT_MIN;

  for (const auto &object: wallObjects_)
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

  const int xOffset = (maxX - minX) / 5;
  const int yOffset = (maxY - minY) / 5;

  safeSpots_.emplace_back(minX + xOffset, minY + yOffset);
  safeSpots_.emplace_back(maxX - xOffset, minY + yOffset);
  safeSpots_.emplace_back(maxX - xOffset, maxY - yOffset);
  safeSpots_.emplace_back(minX + xOffset, maxY - yOffset);

  cv::drawMarker(*MainController::getFrame(), {minX + xOffset, minY + yOffset}, cv::Scalar(0, 0, 255),
                 cv::MARKER_CROSS, 10, 2);

  cv::drawMarker(*MainController::getFrame(), {maxX - xOffset, minY + yOffset}, cv::Scalar(0, 0, 255),
                 cv::MARKER_CROSS, 10, 2);

  cv::drawMarker(*MainController::getFrame(), {maxX - xOffset, maxY - yOffset}, cv::Scalar(0, 0, 255),
                 cv::MARKER_CROSS, 10, 2);

  cv::drawMarker(*MainController::getFrame(), {minX + xOffset, maxY - yOffset}, cv::Scalar(0, 0, 255),
                 cv::MARKER_CROSS, 10, 2);
}

void NavigationController::forceNavigateToGoal()
{
  forceGoToGoal_ = true;
}


bool NavigationController::isAtGoal()
{
  return atGoal_;
}

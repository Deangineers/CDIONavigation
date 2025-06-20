//
// Created by Elias Aggergaard Larsen on 01/05/2025.
//

#ifndef NAVIGATIONCONTROLLER_H
#define NAVIGATIONCONTROLLER_H
#include <atomic>
#include <memory>
#include <string>
#include <vector>
#include "../Models/VectorWithStartPos.h"
#include "../Models/CourseObject.h"
#include "../Models/JourneyModel.h"
#include "../Models/Vector.h"
#include "Utility/ConfigController.h"

class NavigationController
{
public:
  NavigationController() = default;
  void addCourseObject(std::unique_ptr<CourseObject>&& courseObject);
  void addBlockingObject(std::unique_ptr<VectorWithStartPos>&& blockingObject);
  void addCrossObject(std::unique_ptr<VectorWithStartPos>&& blockingObject);
  void clearObjects();
  std::unique_ptr<JourneyModel> calculateDegreesAndDistanceToObject();

  void setHasDeliveredOnce();
  void lastSentCommandWasCompleted();
  void newCommandSent();

private:
  void removeBallsOutsideCourse();
  void removeBallsInsideRobot();
  [[nodiscard]] std::unique_ptr<JourneyModel> makeJourneyModel(const Vector& objectVector, bool toCollectBalls) const;
  [[nodiscard]] Vector navigateToGoal();
  [[nodiscard]] Vector findClosestBall();
  [[nodiscard]] Vector navigateToLeftGoal() const;
  [[nodiscard]] Vector navigateToRightGoal() const;

  Vector handleCollision(Vector objectVector);
  Vector handleObjectNextToBlocking(const CourseObject* courseObject) const;
  Vector handleObjectNearWall(const CourseObject* courseObject, const Vector& vectorToWall) const;
  Vector handleObjectNearCorner(const CourseObject* courseObject,
                                const std::pair<Vector, Vector>& closestVectors) const;
  std::pair<Vector, Vector> getVectorsForClosestBlockingObjects(const CourseObject* courseObject) const;

  [[nodiscard]] bool checkCollisionOnRoute(const Vector& targetVector) const;
  Vector navigateToSafeSpot();
  void findSafeSpots();


  std::vector<std::unique_ptr<CourseObject>> ballVector_;
  std::unique_ptr<CourseObject> goal_;
  std::unique_ptr<CourseObject> robotFront_;
  std::unique_ptr<CourseObject> robotBack_;
  std::vector<std::unique_ptr<VectorWithStartPos>> blockingObjects_;
  std::vector<std::unique_ptr<VectorWithStartPos>> crossObjects_;
  std::unique_ptr<CourseObject> target_;
  std::unique_ptr<CourseObject> potentialTarget_;
  int robotWidth_ = ConfigController::getConfigInt("RobotWidth");

  std::vector<std::pair<int, int>> safeSpots_;
  int currentSafeSpotIndex_ = 0;

  int totalBalls_ = ConfigController::getConfigInt("TotalBalls");
  int robotBallCapacity_ = 5;

  int currentX_ = ConfigController::getConfigInt("safeXLeft");
  int currentY_ = ConfigController::getConfigInt("safeYBot");

  int goToGoalCount_ = 0;
  const int stableThreshold = 3;

  std::atomic_bool hasDeliveredBallsOnce_ = false;
  std::atomic_bool sentShootAt0Balls_ = false;
  bool toCollectBalls_ = true;
  std::atomic_bool atGoal_ = false;
  std::chrono::high_resolution_clock::time_point atGoalTime_;
  bool navigatedToGoalIntermediate_ = false;

  int sameTargetCount_ = 0;
  bool backUpAfterBallPickup_ = false;
  std::atomic_bool lastSentCommandWasCompleted_ = false;

  int amountOfWalls_ = 0;
};


#endif //NAVIGATIONCONTROLLER_H

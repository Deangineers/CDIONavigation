//
// Created by Elias Aggergaard Larsen on 01/05/2025.
//

#ifndef NAVIGATIONCONTROLLER_H
#define NAVIGATIONCONTROLLER_H
#include <memory>
#include <string>
#include <vector>
#include "../Models/CourseObject.h"
#include "../Models/JourneyModel.h"
#include "../Models/Vector.h"
#include "Utility/ConfigController.h"

class NavigationController
{
public:
  NavigationController() = default;
  void addCourseObject(std::unique_ptr<CourseObject>&& courseObject);
  void clearObjects();
  std::unique_ptr<JourneyModel> calculateDegreesAndDistanceToObject();

private:
  void removeBallsOutsideCourse();
  void removeBallsInsideRobot();
  std::unique_ptr<JourneyModel> makeJourneyModel(const Vector& objectVector, bool toCollectBalls);
  [[nodiscard]] Vector navigateToGoal() const;
  [[nodiscard]] Vector findClosestBall() const;
  [[nodiscard]] Vector navigateToLeftGoal() const;
  [[nodiscard]] Vector navigateToRightGoal() const;

  Vector handleCollision(Vector objectVector);

  bool checkCollisionOnRoute(const Vector& targetVector) const;


  std::vector<std::unique_ptr<CourseObject>> ballVector_;
  std::unique_ptr<CourseObject> goal_;
  std::unique_ptr<CourseObject> robotFront_;
  std::unique_ptr<CourseObject> robotBack_;
  std::vector<std::unique_ptr<CourseObject>> blockingObjects_;
  int robotWidth_ = ConfigController::getConfigInt("RobotWidth");

  int totalBalls_ = ConfigController::getConfigInt("TotalBalls");
  int robotBallCapacity_ = 5;

  int currentX_ = ConfigController::getConfigInt("safeXLeft");
  int currentY_ = ConfigController::getConfigInt("safeYBot");
};


#endif //NAVIGATIONCONTROLLER_H

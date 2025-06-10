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
  std::unique_ptr<JourneyModel> makeJourneyModel(const std::pair<int,int>& objectVector, bool toCollectBalls);
  void navigateToGoal(CourseObject* objectToPathTowards, bool &toCollectBalls);
  [[nodiscard]] CourseObject* findClosestBall() const;

  void handleCollision(CourseObject** objectToPathTowards);

  bool checkCollisionOnRoute(const CourseObject* target, const std::pair<int, int>& targetVector) const;


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

  std::unique_ptr<CourseObject> courseObject_;
};


#endif //NAVIGATIONCONTROLLER_H

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

class NavigationController
{
public:
  NavigationController() = default;
  std::unique_ptr<JourneyModel> calculateDegreesAndDistanceToObject();
  void addCourseObject(std::unique_ptr<CourseObject>&& courseObject);
  void clearObjects();
private:
  [[nodiscard]] const CourseObject* findClosestBall() const;
  std::pair<int,int> calculateVectorToObject(const CourseObject* courseObject) const;
  double calculateAngleDifferenceBetweenVectors(const std::pair<int,int>& firstVector, const std::pair<int,int>& secondVector);

  std::vector<std::unique_ptr<CourseObject>> ballVector_;
  std::vector<std::unique_ptr<CourseObject>> eggVector_;
  std::unique_ptr<CourseObject> goal_;
  std::unique_ptr<CourseObject> robotFront_;
  std::unique_ptr<CourseObject> robotBack_;

  int ballsInRobot_ = 0;
  int robotBallCapacity_ = 5;

};



#endif //NAVIGATIONCONTROLLER_H

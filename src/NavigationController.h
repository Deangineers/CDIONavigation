//
// Created by Elias Aggergaard Larsen on 01/05/2025.
//

#ifndef NAVIGATIONCONTROLLER_H
#define NAVIGATIONCONTROLLER_H
#include <memory>
#include <string>
#include <vector>

#include "CourseObject.h"


class NavigationController
{
public:
  static std::string navigate();
  static void addCourseObject(std::unique_ptr<CourseObject>&& courseObject);
  static void clearObjects();
private:
  static std::pair<int,int> calculateVectorToObject(const CourseObject* courseObject);
  static double calculateAngleDifferenceBetweenVectors(const std::pair<int,int>& firstVector, const std::pair<int,int>& secondVector);

  static inline std::vector<std::unique_ptr<CourseObject>> ballVector_;
  static inline std::vector<std::unique_ptr<CourseObject>> eggVector_;
  static inline std::unique_ptr<CourseObject> goal_;
  static inline std::unique_ptr<CourseObject> robotFront_;
  static inline std::unique_ptr<CourseObject> robotBack_;

  static inline int ballsInRobot_ = 0;
  static constexpr int robotBallCapacity_ = 5;

};



#endif //NAVIGATIONCONTROLLER_H

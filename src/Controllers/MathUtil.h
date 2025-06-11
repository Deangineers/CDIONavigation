//
// Created by Elias Aggergaard Larsen on 09/06/2025.
//

#ifndef MATHUTIL_H
#define MATHUTIL_H
#include <utility>
#include "../Models/CourseObject.h"

class MathUtil
{
public:
  static double calculateAngleDifferenceBetweenVectors(const std::pair<int,int>& firstVector, const std::pair<int,int>& secondVector);
  static std::pair<int,int> calculateVectorToObject(const CourseObject* robotFront, const CourseObject* courseObject);
  static CourseObject* getRobotMiddle(const CourseObject* robotBack, const CourseObject* robotFront);
};



#endif //MATHUTIL_H

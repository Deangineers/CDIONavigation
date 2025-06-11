//
// Created by Elias Aggergaard Larsen on 09/06/2025.
//

#ifndef MATHUTIL_H
#define MATHUTIL_H
#include <utility>
#include "../Models/CourseObject.h"
#include "../Models/Vector.h"

class MathUtil
{
public:
  static double calculateAngleDifferenceBetweenVectors(const Vector& firstVector,
                                                       const Vector& secondVector);
  static Vector calculateVectorToObject(const CourseObject* robotFront, const CourseObject* courseObject);
  static CourseObject getRobotMiddle(const CourseObject* robotBack, const CourseObject* robotFront);
};


#endif //MATHUTIL_H

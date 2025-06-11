//
// Created by Elias Aggergaard Larsen on 09/06/2025.
//

#include <cmath>
#include "MathUtil.h"

double MathUtil::calculateAngleDifferenceBetweenVectors(const std::pair<int, int>& firstVector,
                                                        const std::pair<int, int>& secondVector)
{
  int dotProduct = firstVector.first * secondVector.first + firstVector.second * secondVector.second;
  const double vector1Length = std::sqrt(
    firstVector.first * firstVector.first + firstVector.second * firstVector.second);
  const double vector2Length = std::sqrt(
    secondVector.first * secondVector.first + secondVector.second * secondVector.second);
  if (vector1Length == 0.0 || vector2Length == 0.0)
  {
    return 0.0;
  }

  double cosTheta = dotProduct / (vector1Length * vector2Length);
  if (cosTheta < -1.0) cosTheta = -1.0;
  if (cosTheta > 1.0) cosTheta = 1.0;

  double angleRadians = std::acos(cosTheta);
  double crossProduct = firstVector.first * secondVector.second - firstVector.second * secondVector.first;

  if (crossProduct > 0.0)
  {
    angleRadians = -angleRadians;
  }

  double angleDegrees = angleRadians * (180.0 / M_PI);

  return angleDegrees;
}

std::pair<int, int> MathUtil::calculateVectorToObject(const CourseObject* robotFront, const CourseObject* courseObject)
{
  const int middleX = (courseObject->x1() + courseObject->x2()) / 2;
  const int middleY = (courseObject->y1() + courseObject->y2()) / 2;

  const int robotMiddleX = (robotFront->x1() + robotFront->x2()) / 2;
  const int robotMiddleY = (robotFront->y1() + robotFront->y2()) / 2;

  return std::make_pair(middleX - robotMiddleX, middleY - robotMiddleY);
}

CourseObject* MathUtil::getRobotMiddle(const CourseObject* robotBack, const CourseObject* robotFront)
{
  const int middleX1 = (robotFront->x1() + robotBack->x1()) / 2;
  const int middleX2 = (robotFront->x2() + robotBack->x2()) / 2;
  const int middleY1 = (robotFront->y1() + robotBack->y1()) / 2;
  const int middleY2 = (robotFront->y2() + robotBack->y2()) / 2;

  return new CourseObject(middleX1, middleY1, middleX2, middleY2, "");
}



//
// Created by Elias Aggergaard Larsen on 09/06/2025.
//

#include <cmath>
#include "MathUtil.h"

#include "Utility/ConfigController.h"

double MathUtil::calculateAngleDifferenceBetweenVectors(const Vector& firstVector,
                                                        const Vector& secondVector)
{
  int dotProduct = firstVector.x * secondVector.x + firstVector.y * secondVector.y;
  const double vector1Length = std::sqrt(
    firstVector.x * firstVector.x + firstVector.y * firstVector.y);
  const double vector2Length = std::sqrt(
    secondVector.x * secondVector.x + secondVector.y * secondVector.y);
  if (vector1Length == 0.0 || vector2Length == 0.0)
  {
    return 0.0;
  }

  double cosTheta = dotProduct / (vector1Length * vector2Length);
  if (cosTheta < -1.0) cosTheta = -1.0;
  if (cosTheta > 1.0) cosTheta = 1.0;

  double angleRadians = std::acos(cosTheta);
  double crossProduct = firstVector.x * secondVector.y - firstVector.y * secondVector.x;

  if (crossProduct > 0.0)
  {
    angleRadians = -angleRadians;
  }

  double angleDegrees = angleRadians * (180.0 / M_PI);

  return angleDegrees;
}

Vector MathUtil::calculateVectorToObject(const CourseObject* from, const CourseObject* courseObject)
{
  const int middleX = (courseObject->x1() + courseObject->x2()) / 2;
  const int middleY = (courseObject->y1() + courseObject->y2()) / 2;

  const int robotMiddleX = (from->x1() + from->x2()) / 2;
  const int robotMiddleY = (from->y1() + from->y2()) / 2;

  return Vector(middleX - robotMiddleX, middleY - robotMiddleY);
}

CourseObject MathUtil::getRobotMiddle(const CourseObject* robotBack, const CourseObject* robotFront)
{
  const int middleX1 = (robotFront->x1() + robotBack->x1()) / 2;
  const int middleX2 = (robotFront->x2() + robotBack->x2()) / 2;
  const int middleY1 = (robotFront->y1() + robotBack->y1()) / 2;
  const int middleY2 = (robotFront->y2() + robotBack->y2()) / 2;

  const int middleX = (middleX1 + middleX2) / 2;
  const int middleY = (middleY1 + middleY2) / 2;

  return CourseObject(middleX, middleY, middleX, middleY, "");
}

void MathUtil::correctCourseObjectForPerspective(CourseObject* robotBack, CourseObject* robotFront)
{
  auto correct = [&](CourseObject* obj) {
    int middleX = (obj->x1() + obj->x2()) / 2;
    int middleY = (obj->y1() + obj->y2()) / 2;
    int xDiff = imageCenterX_ - middleX;
    int yDiff = imageCenterY_ - middleY;

    double xOffset = xDiff * ConfigController::getConfigInt("PerspectiveOffset")*0.01;
    double yOffset = yDiff * ConfigController::getConfigInt("PerspectiveOffset")*0.01;

    obj->shiftX(xOffset);
    obj->shiftY(yOffset);
  };

  correct(robotBack);
  correct(robotFront);
}



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

Vector MathUtil::calculateVectorToObject(const CourseObject* robotFront, const CourseObject* courseObject)
{
  const int middleX = (courseObject->x1() + courseObject->x2()) / 2;
  const int middleY = (courseObject->y1() + courseObject->y2()) / 2;

  const int robotMiddleX = (robotFront->x1() + robotFront->x2()) / 2;
  const int robotMiddleY = (robotFront->y1() + robotFront->y2()) / 2;

  return Vector(middleX - robotMiddleX, middleY - robotMiddleY);
}

CourseObject MathUtil::getRobotMiddle(const CourseObject* robotBack, const CourseObject* robotFront)
{
  const int middleX1 = (robotFront->x1() + robotBack->x1()) / 2;
  const int middleX2 = (robotFront->x2() + robotBack->x2()) / 2;
  const int middleY1 = (robotFront->y1() + robotBack->y1()) / 2;
  const int middleY2 = (robotFront->y2() + robotBack->y2()) / 2;

  return CourseObject(middleX1, middleY1, middleX2, middleY2, "");
}

void MathUtil::correctCourseObjectForHeightOffset(CourseObject* robotBack, CourseObject* robotFront)
{
  const double robotHeight = ConfigController::getConfigInt("RobotHeightInMM");
  const double robotLength = ConfigController::getConfigInt("RobotLengthInMM");

  int backX = (robotBack->x1() + robotBack->x2()) / 2;
  int backY = (robotBack->y1() + robotBack->y2()) / 2;
  int frontX = (robotFront->x1() + robotFront->x2()) / 2;
  int frontY = (robotFront->y1() + robotFront->y2()) / 2;

  int dx = frontX - backX;
  int dy = frontY - backY;
  double pixelLength = std::sqrt(dx * dx + dy * dy);

  if (pixelLength < 1e-3)
  {
    return;
  }

  double mmPerPixel = robotLength / pixelLength;

  double offsetPixels = robotHeight / mmPerPixel;

  double ux = dx / pixelLength;
  double uy = dy / pixelLength;

  int correctionX = static_cast<int>(std::round(-ux * offsetPixels));
  int correctionY = static_cast<int>(std::round(-uy * offsetPixels));

  robotFront->shiftX(correctionX);
  robotFront->shiftY(correctionY);
  robotBack->shiftX(correctionX);
  robotBack->shiftY(correctionY);
}



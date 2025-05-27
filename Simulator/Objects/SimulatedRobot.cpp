//
// Created by Elias Aggergaard Larsen on 25/05/2025.
//

#include "SimulatedRobot.h"
#include <cmath>
#include "../TextureLocations.h"

SimulatedRobot::SimulatedRobot(EngineBase* engineBase) : robotFront_(std::make_shared<DrawAble>())
                                                         ,robotRear_(std::make_shared<DrawAble>())
{
  engineBase->registerDrawAble(robotFront_);
  engineBase->registerDrawAble(robotRear_);

  constexpr int sizeOfDot = 20;
  constexpr int lengthOfRobot = 50;

  robotFront_->setSize(sizeOfDot,sizeOfDot);
  robotRear_->setSize(sizeOfDot,sizeOfDot);

  robotFront_->setPosition(250,250);
  robotRear_->setPosition(250,250-lengthOfRobot);

  robotFront_->setTextureLocation(&robotFront);
  robotRear_->setTextureLocation(&robotRear);
}

void SimulatedRobot::addRobotToNavController(NavigationController* navController) const
{
  constexpr int widthOfRobot = 50;
  int x1 = robotFront_->getX();
  int y1 = robotFront_->getY();
  int x2 = robotFront_->getX() + widthOfRobot;
  int y2 = robotFront_->getY() + widthOfRobot;
  navController->addCourseObject(std::make_unique<CourseObject>(x1,y1,x2,y2,"robotFront"));

  x1 = robotRear_->getX();
  y1 = robotRear_->getY();
  x2 = robotRear_->getX() + widthOfRobot;
  y2 = robotRear_->getY() + widthOfRobot;
  navController->addCourseObject(std::make_unique<CourseObject>(x1,y1,x2,y2,"robotBack"));
}

void SimulatedRobot::handleCommand(Command command, double deltaTime)
{
  double speed = command.getSpeed();
  if (command.getAction() == "l")
  {
    rotate(speed * rotateSpeedFactor * deltaTime);
  }
  else if (command.getAction() == "r")
  {
    rotate(speed * rotateSpeedFactor * deltaTime * -1);
  }
  else if (command.getAction() == "f" || command.getAction() == "b")
  {
    double x1 = robotFront_->getX();
    double y1 = robotFront_->getY();
    double x2 = robotRear_->getX();
    double y2 = robotRear_->getY();

    double dx = x1 - x2;
    double dy = y1 - y2;

    double length = std::sqrt(dx * dx + dy * dy);
    if (length == 0) return;

    dx /= length;
    dy /= length;

    double distance = speed * deltaTime * movementSpeedFactor;
    if (command.getAction() == "b") distance *= -1;

    double moveX = dx * distance;
    double moveY = dy * distance;

    robotFront_->setPosition(x1 + moveX, y1 + moveY);
    robotRear_->setPosition(x2 + moveX, y2 + moveY);
  }
}

void SimulatedRobot::rotate(double degrees)
{
  double x1 = robotFront_->getX();
  double y1 = robotFront_->getY();
  double x2 = robotRear_->getX();
  double y2 = robotRear_->getY();

  double cx = (x1 + x2) / 2.0;
  double cy = (y1 + y2) / 2.0;

  double radians = degrees * M_PI / 180.0;

  auto rotatePoint = [cx, cy, radians](double x, double y) {
    double dx = x - cx;
    double dy = y - cy;
    double newX = cx + dx * cos(radians) - dy * sin(radians);
    double newY = cy + dx * sin(radians) + dy * cos(radians);
    return std::make_pair(newX, newY);
  };

  auto [newX1, newY1] = rotatePoint(x1, y1);
  auto [newX2, newY2] = rotatePoint(x2, y2);

  robotFront_->setPosition(newX1, newY1);
  robotRear_->setPosition(newX2, newY2);
}

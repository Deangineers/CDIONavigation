//
// Created by Elias Aggergaard Larsen on 25/05/2025.
//

#include "SimulatedRobot.h"

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

  robotFront_->setTextureLocation(&greenBox);
  robotRear_->setTextureLocation(&greenBox);
}

void SimulatedRobot::addRobotToNavController(NavigationController* navController) const
{
  int x1 = robotFront_->getX();
  int y1 = robotFront_->getY();
  int x2 = robotRear_->getX();
  int y2 = robotRear_->getY();
  navController->addCourseObject(std::make_unique<CourseObject>(x1,y1,x2,y2,"robotFront"));

  x1 = robotRear_->getX();
  y1 = robotRear_->getY();
  x2 = robotRear_->getX();
  y2 = robotRear_->getY();
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
    // 1. Get current positions
    double x1 = robotFront_->getX();
    double y1 = robotFront_->getY();
    double x2 = robotRear_->getX();
    double y2 = robotRear_->getY();

    // 2. Calculate direction vector from rear to front
    double dx = x1 - x2;
    double dy = y1 - y2;

    // 3. Normalize direction
    double length = std::sqrt(dx * dx + dy * dy);
    if (length == 0) return; // prevent division by zero

    dx /= length;
    dy /= length;

    // 4. Calculate movement distance
    double distance = speed * deltaTime;
    if (command.getAction() == "b") distance *= -1;

    // 5. Move both points in the direction
    double moveX = dx * distance;
    double moveY = dy * distance;

    robotFront_->setPosition(x1 + moveX, y1 + moveY);
    robotRear_->setPosition(x2 + moveX, y2 + moveY);
  }
}

void SimulatedRobot::rotate(double degrees)
{
  // Step 1: Get current positions
  double x1 = robotFront_->getX();
  double y1 = robotFront_->getY();
  double x2 = robotRear_->getX();
  double y2 = robotRear_->getY();

  // Step 2: Find center of rotation (midpoint)
  double cx = (x1 + x2) / 2.0;
  double cy = (y1 + y2) / 2.0;

  // Step 3: Convert degrees to radians
  double radians = degrees * M_PI / 180.0;

  // Step 4: Rotate each point around the center
  auto rotatePoint = [cx, cy, radians](double x, double y) {
    double dx = x - cx;
    double dy = y - cy;
    double newX = cx + dx * cos(radians) - dy * sin(radians);
    double newY = cy + dx * sin(radians) + dy * cos(radians);
    return std::make_pair(newX, newY);
  };

  auto [newX1, newY1] = rotatePoint(x1, y1);
  auto [newX2, newY2] = rotatePoint(x2, y2);

  // Step 5: Update positions
  robotFront_->setPosition(newX1, newY1);
  robotRear_->setPosition(newX2, newY2);
}

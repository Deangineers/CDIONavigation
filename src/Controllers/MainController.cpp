//
// Created by eal on 01/05/25.
//

#include "MainController.h"

#include <sstream>
#include <thread>
#include <opencv2/core/mat.hpp>

#include "../Models/Command.h"
#include "../Models/JourneyModel.h"
#include "Clients/LinuxClient.h"
#include "Clients/MockClient.h"
#include "Utility/ConfigController.h"
#include "Utility/Utility.h"

void MainController::init()
{
  ConfigController::loadConfig("../config.json");
  Utility::writeToFile("log.txt", "");
  clientController_ = std::make_unique<ClientController>(std::make_unique<LinuxClient>());
  navigationController_ = std::make_unique<NavigationController>();
}

void MainController::mockInit()
{
  ConfigController::loadConfig("../config.json");
  clientController_ = std::make_unique<ClientController>(std::make_unique<MockClient>());
  Utility::writeToFile("log.txt", "");
  navigationController_ = std::make_unique<NavigationController>();
}

void MainController::testInit()
{
  ConfigController::loadConfig("../../config.json");
  clientController_ = std::make_unique<ClientController>(std::make_unique<MockClient>());
  Utility::writeToFile("log.txt", "");
  navigationController_ = std::make_unique<NavigationController>();
}

void MainController::addCourseObject(std::unique_ptr<CourseObject>&& courseObject)
{
  navigationController_->addCourseObject(std::move(courseObject));
}

void MainController::addBlockedObject(std::unique_ptr<VectorWithStartPos> blockingObject)
{
  navigationController_->addBlockingObject(std::move(blockingObject));
}

void MainController::addCrossObject(std::unique_ptr<VectorWithStartPos> blockingObject)
{
  navigationController_->addCrossObject(std::move(blockingObject));
}

void MainController::navigateAndSendCommand(cv::Mat* frame)
{
  frame_ = frame;
  auto journey = navigationController_->calculateDegreesAndDistanceToObject();
  navigationController_->clearObjects();

  if (journey == nullptr)
  {
    Utility::appendToFile("log.txt", "Journey was nullptr\n");
    return;
  }
  Utility::appendToFile(
    "log.txt",
    "Journey: " + std::to_string(std::abs(journey->angle)) + ", " + std::to_string(journey->distance) + "\n");

  clientController_->sendCommand(journeyToCommand(journey.get()));
  navigationController_->newCommandSent();
}

Command MainController::journeyToCommand(const JourneyModel* journey)
{
  Command command;
  if (journey->angle == 0 && journey->distance == 0)
  {
    command.setAction("f");
    command.setBallCollection(false);
    command.setSpeed(0);
    command.setDistanceOrAngle(0);
    return command;
  }
  int allowedAngleDiff = ConfigController::getConfigInt("AllowedAngleDifference");
  command.setBallCollection(journey->collectBalls);
  if (journey->angle > allowedAngleDiff || journey->angle < -allowedAngleDiff)
  {
    if (journey->angle > 0)
    {
      command.setAction("l");
    }
    else
    {
      command.setAction("r");
    }
    int maxAngleBeforeSlowingDown = ConfigController::getConfigInt("MaxAngleBeforeSlowingDown");
    if (journey->angle > -maxAngleBeforeSlowingDown && journey->angle < maxAngleBeforeSlowingDown)
    {
      command.setSpeed(ConfigController::getConfigInt("RotationSlowSpeed"));
    }
    else
    {
      command.setSpeed(ConfigController::getConfigInt("RotationFastSpeed"));
    }
    command.setDistanceOrAngle(std::abs(journey->angle));
    return command;
  }

  if (journey->distance != 0.0)
  {
    command.setAction("f");

    if (journey->distance > ConfigController::getConfigInt("FastSpeedMinimumDistance"))
    {
      command.setSpeed(ConfigController::getConfigInt("ForwardFastSpeed"));
    }
    else
    {
      command.setSpeed(ConfigController::getConfigInt("ForwardSlowSpeed"));
    }
    if (journey->distance > ConfigController::getConfigInt("DistanceBeforeSmallBit"))
    {
      command.setDistanceOrAngle(ConfigController::getConfigInt("SmallBit"));
    }
    else
    {
      command.setDistanceOrAngle(journey->distance);
    }
    return command;
  }

  command.setAction("s");
  return command;
}

cv::Mat* MainController::getFrame()
{
  return frame_;
}

void MainController::completedGoalDelivery()
{
  navigationController_->setHasDeliveredOnce();
}

void MainController::completedCommand()
{
  navigationController_->lastSentCommandWasCompleted();
}

int MainController::findMaxValue(const int* cords, const int size, int maxValueAllowed)
{
  if (size == 0)
  {
    return -1;
  }
  int max = cords[0];
  for (int i = 0; i < size; i++)
  {
    if (cords[i] > max && cords[i] < maxValueAllowed)
    {
      max = cords[i];
    }
  }
  return max;
}

int MainController::findMinValue(const int* cords, int size, int minValueAllowed)
{
  if (size == 0)
  {
    return -1;
  }
  int min = cords[0];
  for (int i = 0; i < size; i++)
  {
    if (cords[i] < min && cords[i] > minValueAllowed)
    {
      min = cords[i];
    }
  }
  return min;
}

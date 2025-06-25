//
// Created by eal on 01/05/25.
//

#include "MainController.h"

#include <sstream>
#include <thread>
#include <opencv2/imgproc.hpp>
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

void MainController::addGoalObject(std::unique_ptr<CourseObject>&& goalObject)
{
  navigationController_->addGoalObject(std::move(goalObject));
}

void MainController::navigateAndSendCommand(cv::Mat* frame)
{
  frame_ = frame;
  auto journey = navigationController_->calculateDegreesAndDistanceToObject();
  navigationController_->clearObjects();
  cv::putText(*frame_,std::to_string(amountOfNullVectors_),{50,50},1,1,cv::Scalar(0,0,255));

  if (journey == nullptr && not navigationController_->isAtGoal() && not commandInProgress_)
  {
    amountOfNullVectors_++;
    if (amountOfNullVectors_ > 50)
    {
      journey = std::make_unique<JourneyModel>(10,0,true);
    }
  }

  if (journey != nullptr)
  {
    amountOfNullVectors_ = 0;
  }
  else
  {
    return;
  }
  Utility::appendToFile(
    "log.txt",
    "Journey: " + std::to_string(std::abs(journey->angle)) + ", " + std::to_string(journey->distance) + "\n");

  clientController_->sendCommand(journeyToCommand(journey.get()));
  navigationController_->newCommandSent();
  commandInProgress_ = false;
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
    double angle = std::abs(journey->angle);
    if (angle > 45)
    {
      command.setDistanceOrAngle(angle - 45);
      command.setSpeed(250);
    }
    else if (angle > 20)
    {
      command.setDistanceOrAngle(angle - 20);
      command.setSpeed(150);
    }
    else if (angle > 7)
    {
      command.setDistanceOrAngle(angle);
      command.setSpeed(100);
    }
    else
    {
      command.setDistanceOrAngle(angle);
      command.setSpeed(30);
    }
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
    if (journey->isSafeSpot)
    {
      command.setSpeed(1000);
      command.setDistanceOrAngle(journey->distance);
    }
    else if (journey->isIntermediate)
    {
      command.setSpeed(350);
    }
    else if (journey->isCross)
    {
      command.setSpeed(50);
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
  commandInProgress_ = false;
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

void MainController::forceNavigateToGoal() {
  navigationController_->forceNavigateToGoal();
}


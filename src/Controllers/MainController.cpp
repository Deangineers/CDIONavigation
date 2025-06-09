//
// Created by eal on 01/05/25.
//

#include "MainController.h"

#include <sstream>
#include <thread>

#include "../Models/Command.h"
#include "../Models/JourneyModel.h"
#include "Clients/LinuxClient.h"
#include "Clients/MockClient.h"
#include "Utility/ConfigController.h"

void MainController::init()
{
  ConfigController::loadConfig("config.json");
  client_ = std::make_unique<LinuxClient>();
  navigationController_ = std::make_unique<NavigationController>();
}

void MainController::mockInit()
{
  ConfigController::loadConfig("config.json");
  client_ = std::make_unique<MockClient>();
  navigationController_ = std::make_unique<NavigationController>();
}

void MainController::addCourseObject(std::unique_ptr<CourseObject>&& courseObject)
{
  navigationController_->addCourseObject(std::move(courseObject));
}

void MainController::navigateAndSendCommand()
{
  auto journey = navigationController_->calculateDegreesAndDistanceToObject();
  navigationController_->clearObjects();

  if (journey == nullptr)
  {
    std::cout << "Journey was nullptr\n";
    return;
  }
  std::cout << "Journey: " << std::abs(journey->angle) << ", " << journey->distance << std::endl;

  auto navigationCommand = journeyToCommand(journey.get());
  auto ballCollectionCommand = handleBallCollectionMotor(journey.get());

  client_->sendCommandAndAddNewLine(journeyToCommand(journey.get()) + " " + ballCollectionCommand.formatToSend());
}

Command MainController::handleBallCollectionMotor(const JourneyModel* journey)
{
  Command command;
  command.setMotor("");
  if (journey->collectBalls)
  {
    command.setAction("in");
  }
  else
  {
    command.setAction("out");
  }
  return command;
}

std::string MainController::journeyToCommand(const JourneyModel* journey)
{
  std::stringstream command;
  int allowedAngleDiff = ConfigController::getConfigInt("AllowedAngleDifference");
  if ( journey->angle > allowedAngleDiff || journey->angle < -allowedAngleDiff)
  {
    if (journey->angle > 0)
    {
      command << "l";
    }
    else
    {
      command << "r";
    }
    command << " ";
    int maxAngleBeforeSlowingDown = ConfigController::getConfigInt("MaxAngleBeforeSlowingDown");
    if (journey->angle > - maxAngleBeforeSlowingDown && journey->angle < maxAngleBeforeSlowingDown)
    {
      command << std::to_string(ConfigController::getConfigInt("RotationSlowSpeed"));
    }
    else
    {
      command << std::to_string(ConfigController::getConfigInt("RotationFastSpeed"));
    }
    command << " ";
    command << std::to_string(std::abs(journey->angle));
    return command.str();
  }

  if (journey->distance > 0.0)
  {
    command << "f ";

    if (journey->distance < 30.0)
    {
      command << std::to_string(ConfigController::getConfigInt("ForwardFastSpeed"));
    }
    else
    {
      command << std::to_string(ConfigController::getConfigInt("ForwardSlowSpeed"));
    }
    command << " ";
    if (journey->distance < 25.0)
    {
      command << "20";
    }
    else
    {
      command << std::to_string(journey->distance);
    }
    return command.str();
  }
  command << "s";
  return command.str();
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
    if (cords[i] > min && cords[i] > minValueAllowed)
    {
      min = cords[i];
    }
  }
  return min;
}

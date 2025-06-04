//
// Created by eal on 01/05/25.
//

#include "MainController.h"

#include "../Models/Command.h"
#include "../Models/JourneyModel.h"
#include "Clients/LinuxClient.h"
#include "Clients/MockClient.h"
#include "Utility/ConfigController.h"

void MainController::init()
{
  ConfigController::loadConfig("config.json");
  navigationController_ = std::make_unique<NavigationController>();
}

void MainController::mockInit()
{
  ConfigController::loadConfig("config.json");
  navigationController_ = std::make_unique<NavigationController>();
}

NavigationController* MainController::getNavController()
{
  return navigationController_.get();
}

const char* MainController::navigateAndSendCommand()
{
  auto journey = navigationController_->calculateDegreesAndDistanceToObject();
  navigationController_->clearObjects();
  if (journey == nullptr)
  {
    return "";
  }

  auto ballCollectionCommand = handleBallCollectionMotor(journey.get());
  auto navigationCommand = journeyToCommand(journey.get());

  return (ballCollectionCommand.formatToSend() + "/" + navigationCommand.formatToSend()).c_str();
}

Command MainController::handleBallCollectionMotor(const JourneyModel* journey)
{
  Command command;
  command.setMotor("c");

  if (journey->distance < ConfigController::getConfigInt("DistanceBeforeBallCollection"))
  {
    if (journey->collectBalls)
    {
      command.setAction("in");
    }
    else
    {
      command.setAction("out");
    }
  }
  else
  {
    command.setAction("s");
  }
  return command;
}

Command MainController::journeyToCommand(const JourneyModel* journey)
{
  Command command;
  int allowedAngleDiff = ConfigController::getConfigInt("AllowedAngleDifference");
  if ( journey->angle > allowedAngleDiff || journey->angle < -allowedAngleDiff)
  {
    if (journey->angle > 0)
    {
      command.setAction("r");
    }
    else
    {
      command.setAction("l");
    }
    int maxAngleBeforeSlowingDown = ConfigController::getConfigInt("MaxAngleBeforeSlowingDown");
    if (journey->angle > - maxAngleBeforeSlowingDown && journey->angle < maxAngleBeforeSlowingDown)
    {
      command.setSpeed(ConfigController::getConfigInt("RotationSlowSpeed"));
    }
    else
    {
      command.setSpeed(ConfigController::getConfigInt("RotationFastSpeed"));
    }
    return command;
  }

  if (journey->distance > 0.0)
  {
    command.setAction("f");

    if (journey->distance < 30.0)
    {
      command.setSpeed(ConfigController::getConfigInt("ForwardFastSpeed"));
    }
    else
    {
      command.setSpeed(ConfigController::getConfigInt("ForwardSlowSpeed"));
    }
    return command;
  }
  command.setAction("s");
  return command;
}

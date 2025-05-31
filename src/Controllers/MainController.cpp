//
// Created by eal on 01/05/25.
//

#include "MainController.h"

#include "../Models/Command.h"
#include "../Models/JourneyModel.h"

void MainController::init()
{
  navigationController_ = std::make_unique<NavigationController>();
  client_ = std::make_unique<Client>();
}

NavigationController* MainController::getNavController()
{
  return navigationController_.get();
}

void MainController::navigateAndSendCommand()
{
  auto journey = navigationController_->calculateDegreesAndDistanceToObject();
  if (journey == nullptr) return;

  auto ballCollectionCommand = handleBallCollectionMotor(journey.get());
  auto navigationCommand = journeyToCommand(journey.get());

  client_->sendCommand(ballCollectionCommand.formatToSend());
  client_->sendCommand(navigationCommand.formatToSend());
  navigationController_->clearObjects();
}

Command MainController::handleBallCollectionMotor(const JourneyModel* journey)
{
  Command command;
  command.setMotor("c");

  if (journey->distance < 30)
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
    command.setAction("c");
  }
  return command;
}

Command MainController::journeyToCommand(const JourneyModel* journey)
{
  Command command;
  if ( journey->angle > 5 || journey->angle < -5)
  {
    if (journey->angle > 0)
    {
      command.setAction("r");
    }
    else
    {
      command.setAction("l");
    }

    if (journey->angle > - 20.0 && journey->angle < 20.0)
    {
      command.setSpeed(100);
    }
    else
    {
      command.setSpeed(500);
    }
    return command;
  }

  if (journey->distance > 0.0)
  {
    command.setAction("f");

    if (journey->distance < 30.0)
    {
      command.setSpeed(100);
    }
    else
    {
      command.setSpeed(500);
    }
    return command;
  }
  command.setAction("s");
  return command;
}

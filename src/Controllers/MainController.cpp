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
  Command command;
  auto journey = navigationController_->calculateDegreesAndDistanceToObject();
  if ( journey->angle > 1 || journey->angle < -1)
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
    client_->sendCommand(command.formatToSend());
    return;
  }

  if (journey->distance > 0.0)
  {
    command.setAction("f");

    if (journey->distance < 30.0)
    {
      updateBallCollectionAction("in");
      command.setSpeed(100);
    }
    else
    {
      command.setSpeed(500);
    }
    client_->sendCommand(command.formatToSend());
    return;
  }

  updateBallCollectionAction("s");
  command.setAction("s");
  client_->sendCommand(command.formatToSend());
  //TODO refine navigation, as we are just driving to the object and stopping at the moment
}

void MainController::updateBallCollectionAction(const std::string& action)
{
  Command command;
  command.setAction(action);
  command.setMotor("c");
  client_->sendCommand(command.formatToSend());
}

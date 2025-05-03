//
// Created by eal on 01/05/25.
//

#include "MainController.h"

#include "../Models/Command.h"

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
  std::pair<double, double> degreesAndDistanceToObject = navigationController_->calculateDegreesAndDistanceToObject();
  double degreesToTurn = degreesAndDistanceToObject.first;
  double distanceToObject = degreesAndDistanceToObject.second;
  if (degreesToTurn > 1 || degreesToTurn < -1)
  {
    if (degreesToTurn > 0)
    {
      command.setAction("r");
    }
    else
    {
      command.setAction("l");
    }

    if (degreesToTurn > - 20.0 && degreesToTurn < 20.0)
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

  if (distanceToObject > 0.0)
  {
    command.setAction("f");

    if (distanceToObject < 30.0)
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

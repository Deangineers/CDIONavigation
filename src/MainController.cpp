//
// Created by eal on 01/05/25.
//

#include "MainController.h"

#include "Command.h"

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
  auto command = std::make_unique<Command>();
  std::pair<double, double> degreesAndDistanceToObject = navigationController_->calculateDegreesAndDistanceToObject();
  double degreesToTurn = degreesAndDistanceToObject.first;
  double distanceToObject = degreesAndDistanceToObject.second;
  if (degreesToTurn != 0.0)
  {
    command->setAction("r");
    if (degreesToTurn > - 20.0 && degreesToTurn < 20.0)
    {
      command->setSpeed(100);
    }
    else
    {
      command->setSpeed(500);
    }
    client_->sendCommand(command->formatToSend());
    return;
  }

  if (distanceToObject > 0.0)
  {
    command->setAction("f");

    if (distanceToObject < 30.0)
    {
      updateBallCollectionAction("in");
      command->setSpeed(100);
    }
    else
    {
      command->setSpeed(500);
    }
    client_->sendCommand(command->formatToSend());
    return;
  }

  updateBallCollectionAction("s");
  command->setAction("s");
  client_->sendCommand(command->formatToSend());
  //TODO refine navigation, as we are just driving to the object and stopping at the moment
}

void MainController::updateBallCollectionAction(const std::string& action)
{
  const auto command = std::make_unique<Command>();
  command->setAction(action);
  command->setMotor("c");
  client_->sendCommand(command->formatToSend());
}

//
// Created by eal on 01/05/25.
//

#include "MainController.h"

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
  std::pair<double, double> degreesAndDistanceToObject = navigationController_->calculateDegreesAndDistanceToObject();
  double degreesToTurn = degreesAndDistanceToObject.first;
  double distanceToObject = degreesAndDistanceToObject.second;

  //TODO calculate to command and send it
}

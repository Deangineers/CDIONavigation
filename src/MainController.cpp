//
// Created by eal on 01/05/25.
//

#include "MainController.h"

void MainController::init()
{
  navigationController_ = std::make_unique<NavigationController>();
}

NavigationController* MainController::getNavController()
{
  return navigationController_.get();
}

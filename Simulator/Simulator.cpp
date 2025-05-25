//
// Created by Elias Aggergaard Larsen on 25/05/2025.
//

#include "Simulator.h"

Simulator::Simulator() : engineBase_(std::make_unique<EngineBase>()), navController_(std::make_unique<NavigationController>())
, robot_(std::make_unique<SimulatedRobot>(engineBase_.get())), ball_(std::make_unique<Ball>(engineBase_.get()))
{
  engineBase_->launch();
}

void Simulator::update(double deltaTime)
{
  navController_->clearObjects();
  robot_->addRobotToNavController(navController_.get());
  ball_->addToNavController(navController_.get());

  auto journey = navController_->calculateDegreesAndDistanceToObject();

}

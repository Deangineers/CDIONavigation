//
// Created by Elias Aggergaard Larsen on 25/05/2025.
//

#include "Simulator.h"

#include <thread>

#include "../src/Controllers/MainController.h"

Simulator::Simulator() : engineBase_(std::make_unique<EngineBase>()), navController_(std::make_unique<NavigationController>())
                         , robot_(std::make_unique<SimulatedRobot>(engineBase_.get())), ball_(std::make_unique<Ball>(engineBase_.get()))
                         , cross_(std::make_unique<Cross>(engineBase_.get()))
{
  engineBase_->registerUpdateFunction([this](double deltaTime)->void
    {update(deltaTime);}
    );
  engineBase_->getGraphicsLibrary()->setTargetFPS(120);
  engineBase_->launch();
}

void Simulator::update(double deltaTime)
{
  handleClicks();
  navController_->clearObjects();
  robot_->addRobotToNavController(navController_.get());
  ball_->addToNavController(navController_.get());
  cross_->addToNavController(navController_.get());

  auto journey = navController_->calculateDegreesAndDistanceToObject();
  if (journey != nullptr)
  {
    robot_->handleCommand(MainController::journeyToCommand(journey.get()),deltaTime);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void Simulator::handleClicks()
{
  if (not lButtonDown && engineBase_->getGraphicsLibrary()->isMouseButtonDown(ENGINEBASE_BUTTON_LEFT))
  {
    lButtonDown = true;

    const auto mousePos = engineBase_->getGraphicsLibrary()->getMousePos();
    ball_->handleMouseClick(mousePos);
  }
  else if (lButtonDown && not engineBase_->getGraphicsLibrary()->isMouseButtonDown(ENGINEBASE_BUTTON_LEFT))
  {
    lButtonDown = false;
  }
}

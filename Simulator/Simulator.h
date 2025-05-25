//
// Created by Elias Aggergaard Larsen on 25/05/2025.
//

#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <memory>

#include "Objects/SimulatedRobot.h"
#include "../src/Controllers/NavigationController.h"
#include "EngineBase/EngineBase.h"
#include "Objects/Ball.h"


class Simulator
{
public:
  Simulator();
  ~Simulator() = default;
  void update(double deltaTime);
private:
  std::unique_ptr<EngineBase> engineBase_;
  std::unique_ptr<NavigationController> navController_;
  std::unique_ptr<SimulatedRobot> robot_;
  std::unique_ptr<Ball> ball_;
};



#endif //SIMULATOR_H

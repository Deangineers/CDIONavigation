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
#include "Objects/Cross.h"
#include "Objects/Egg.h"


class Simulator
{
public:
  Simulator();
  ~Simulator() = default;
  void update(double deltaTime);
private:
  void handleClicks();
  std::unique_ptr<EngineBase> engineBase_;
  std::unique_ptr<NavigationController> navController_;
  std::unique_ptr<SimulatedRobot> robot_;
  std::unique_ptr<Ball> ball_;
  std::unique_ptr<Cross> cross_;
  std::unique_ptr<Egg> egg_;
  bool lButtonDown = false;
  bool rButtonDown = false;
};



#endif //SIMULATOR_H

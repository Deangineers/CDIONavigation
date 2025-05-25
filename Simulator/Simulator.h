//
// Created by Elias Aggergaard Larsen on 25/05/2025.
//

#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <memory>

#include "SimulatedRobot.h"
#include "EngineBase/EngineBase.h"


class Simulator
{
public:
  Simulator();
  ~Simulator() = default;
  void update(double deltaTime);
private:
  std::shared_ptr<EngineBase> engineBase_;
  std::shared_ptr<SimulatedRobot> robot_;
};



#endif //SIMULATOR_H

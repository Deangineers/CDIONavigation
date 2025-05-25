//
// Created by Elias Aggergaard Larsen on 25/05/2025.
//

#include "Simulator.h"

Simulator::Simulator() : engineBase_(std::make_shared<EngineBase>()), robot_(std::make_shared<SimulatedRobot>(engineBase_.get()))
{
  engineBase_->launch();
}

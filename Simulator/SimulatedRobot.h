//
// Created by Elias Aggergaard Larsen on 25/05/2025.
//

#ifndef SIMULATEDROBOT_H
#define SIMULATEDROBOT_H
#include "EngineBase/EngineBase.h"


class SimulatedRobot
{
public:
  explicit SimulatedRobot(EngineBase* engineBase);
private:
  std::shared_ptr<DrawAble> robotFront_;
  std::shared_ptr<DrawAble> robotRear_;
};



#endif //SIMULATEDROBOT_H

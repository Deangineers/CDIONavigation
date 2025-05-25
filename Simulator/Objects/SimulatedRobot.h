//
// Created by Elias Aggergaard Larsen on 25/05/2025.
//

#ifndef SIMULATEDROBOT_H
#define SIMULATEDROBOT_H
#include "EngineBase/EngineBase.h"
#include "../../src/Controllers/NavigationController.h"

class SimulatedRobot
{
public:
  explicit SimulatedRobot(EngineBase* engineBase);
  void addRobotToNavController(NavigationController* navController) const;
private:
  double movementSpeedFactor = 1;
  std::shared_ptr<DrawAble> robotFront_;
  std::shared_ptr<DrawAble> robotRear_;
};



#endif //SIMULATEDROBOT_H

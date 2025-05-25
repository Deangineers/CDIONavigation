//
// Created by Elias Aggergaard Larsen on 25/05/2025.
//

#include "SimulatedRobot.h"

#include "TextureLocations.h"

SimulatedRobot::SimulatedRobot(EngineBase* engineBase) : robotFront_(std::make_shared<DrawAble>())
                                                         ,robotRear_(std::make_shared<DrawAble>())
{
  engineBase->registerDrawAble(robotFront_);
  engineBase->registerDrawAble(robotRear_);

  constexpr int sizeOfDot = 20;
  constexpr int lengthOfRobot = 50;

  robotFront_->setSize(sizeOfDot,sizeOfDot);
  robotRear_->setSize(sizeOfDot,sizeOfDot);

  robotFront_->setPosition(250,250);
  robotRear_->setPosition(250,250-lengthOfRobot);

  robotFront_->setTextureLocation(&greenBox);
  robotRear_->setTextureLocation(&greenBox);
}

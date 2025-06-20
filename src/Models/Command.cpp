//
// Created by Emil Damholt Savino on 01/05/2025.
//

#include "Command.h"

#include "../../Simulator/TextureLocations.h"

Command::Command() : speed_(0), distanceOrAngle_(0), ballCollectionGrabsBalls_(true)
{
}

Command::Command(const std::string& action, const int speed, const double distanceOrAngle, bool ballCollection)
  : speed_(speed), distanceOrAngle_(distanceOrAngle), ballCollectionGrabsBalls_(ballCollection)
{
  this->action_ = action;
  this->speed_ = speed;
}

void Command::setAction(const std::string& action)
{
  this->action_ = action;
}

void Command::setSpeed(int speed)
{
  this->speed_ = speed;
}

void Command::setDistanceOrAngle(const double distanceOrAngle)
{
  this->distanceOrAngle_ = distanceOrAngle;
}

void Command::setBallCollection(const bool ballCollectionStatus)
{
  this->ballCollectionGrabsBalls_ = ballCollectionStatus;
}

std::string Command::formatToSend() const
{
  if (not ballCollectionGrabsBalls_)
  {
    return "out";
  }
  std::string sendString = action_;
  sendString += " " + std::to_string(speed_);
  sendString += " " + std::to_string(distanceOrAngle_);

  if (ballCollectionGrabsBalls_)
  {
    sendString += " in";
  }
  else
  {
    sendString += " out";
  }

  return sendString;
}

std::string Command::getAction() const
{
  return action_;
}

int Command::getSpeed() const
{
  return speed_;
}

double Command::getDistanceOrAngle() const
{
  return distanceOrAngle_;
}

bool Command::getBallCollectionGrabsBalls() const
{
  return ballCollectionGrabsBalls_;
}


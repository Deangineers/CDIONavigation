//
// Created by Emil Damholt Savino on 01/05/2025.
//

#include "Command.h"

Command::Command()
{
  this->action_ = "";
  this->speed_ = 0;
  this->motor_ = "";

}

Command::Command(const std::string& action, const int speed, const std::string& motor)
{
  this->action_ = action;
  this->speed_ = speed;
  this->motor_ = motor;
}

void Command::setAction(const std::string& action)
{
  this->action_ = action;
}

void Command::setSpeed(int speed)
{
  this->speed_ = speed;
}

void Command::setMotor(const std::string& motor)
{
  this->motor_ = motor;
}

std::string Command::formatToSend() const
{
  std::string sendString = action_;
  if (speed_ > 0)
  {
    sendString += " " + std::to_string(speed_);
  }

  if (!motor_.empty())
  {
    sendString += " " + motor_;
  }

  return sendString;
}

std::string& Command::getAction()
{
  return action_;
}

int Command::getSpeed() const
{
  return speed_;
}

std::string& Command::getMotor()
{
  return motor_;
}

//
// Created by Emil Damholt Savino on 01/05/2025.
//

#include "Command.h"

Command::Command()
{
  this->action = "";
  this->speed = 0;
  this->motor = "";

}

Command::Command(const std::string& action, const int speed, const std::string& motor)
{
  this->action = action;
  this->speed = speed;
  this->motor = motor;
}

void Command::setAction(const std::string& action)
{
  this->action = action;
}

void Command::setSpeed(int speed)
{
  this->speed = speed;
}

void Command::setMotor(const std::string& motor)
{
  this->motor = motor;
}

std::string Command::formatToSend() const
{
  std::string sendString = action;
  if (speed > 0)
  {
    sendString += " " + std::to_string(speed);
  }

  if (!motor.empty())
  {
    sendString += " " + motor;
  }

  return sendString;
}

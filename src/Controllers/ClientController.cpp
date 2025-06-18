//
// Created by Elias Aggergaard Larsen on 10/06/2025.
//

#include "ClientController.h"

#include "Utility/ConfigController.h"
#include "Utility/Utility.h"

ClientController::ClientController(std::unique_ptr<IClient> client) : client_(std::move(client))
{
  commands_.reserve(ConfigController::getConfigInt("AmountOfCommandsToAverage"));
}

void ClientController::sendCommand(Command command)
{
  if (ConfigController::getConfigBool("ToAverageCommandsInClientController"))
  {
    client_->sendCommandAndAddNewLine(command.formatToSend());
    return;
  }
  commands_.emplace_back(command);
  if (commands_.size() == ConfigController::getConfigInt("AmountOfCommandsToAverage"))
  {
    std::string action = command.getAction();
    int speed = command.getSpeed();
    double distanceOrAngle = command.getDistanceOrAngle();
    bool bc = command.getBallCollectionGrabsBalls();
    double distanceOrAngleSum = 0;

    for (auto& loopCommand : commands_)
    {
      distanceOrAngleSum += loopCommand.getDistanceOrAngle();
      if (loopCommand.getAction() != action)
      {
        clearCommands();
        return;
      }

      if (loopCommand.getAction() == "r" || loopCommand.getAction() == "l")
      {
        int angleDiffAllowed = ConfigController::getConfigInt("MaxDifferenceInAngle");
        if (std::abs(distanceOrAngle - loopCommand.getDistanceOrAngle()) > angleDiffAllowed)
        {
          clearCommands();
          return;
        }
      }
      else
      {
        int distanceDiffAllowed = ConfigController::getConfigInt("MaxDifferenceInDistance");
        if (std::abs(distanceOrAngle - loopCommand.getDistanceOrAngle()) > distanceDiffAllowed)
        {
          clearCommands();
          return;
        }
      }
      if (bc != loopCommand.getBallCollectionGrabsBalls())
      {
        clearCommands();
        return;
      }
    }
    double averageDistanceOrAngle = distanceOrAngleSum / static_cast<double>(ConfigController::getConfigInt(
      "AmountOfCommandsToAverage"));
    command.setDistanceOrAngle(averageDistanceOrAngle);
    client_->sendCommandAndAddNewLine(command.formatToSend());
    commands_.clear();
    commands_.reserve(ConfigController::getConfigInt("AmountOfCommandsToAverage"));
  }
  if (commands_.size() > ConfigController::getConfigInt("AmountOfCommandsToAverage"))
  {
    Utility::appendToFile("log.txt", "Commands.Size was too large\n");
    clearCommands();
  }
}

void ClientController::clearCommands()
{
  Utility::appendToFile("log.txt", "Clearing commands and retrying\n");
  commands_.clear();
  commands_.reserve(ConfigController::getConfigInt("AmountOfCommandsToAverage"));
}

//
// Created by Emil Damholt Savino on 01/05/2025.
//

#include "LinuxClient.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fstream>
#include <string>
#include <thread>

#include "../MainController.h"
#include "Utility/ConfigController.h"
#include "Utility/DebugController.h"
#include "Utility/Utility.h"

LinuxClient::LinuxClient() : commandToSend_(""), keepRunning_(true), firstRecieved_(false)
{
  LinuxClient::connectToServer();
  commandToSend_ = ("f 30 17 in\n");
  sendThread_ = std::thread(&LinuxClient::sendCommand, this);
}

LinuxClient::~LinuxClient()
{
  keepRunning_ = false;
  sendThread_.join();
}

void LinuxClient::sendCommandAndAddNewLine(const std::string& command)
{
  std::lock_guard lock(mutex_);
  if (not firstRecieved_)
  {
    return;
  }
  commandToSend_ = command + "\n";
}

void LinuxClient::sendBallCollectionCommand(const std::string& command)
{
  std::lock_guard lock(mutex_);
  if (command == lastBallCollectionCommand_)
  {
    return;
  }
  ballCollectionCommand_ = command;
}

void LinuxClient::connectToServer()
{
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0)
  {
    std::cerr << "Failed to create socket\n";
    exit(EXIT_FAILURE);
  }

  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(ConfigController::getConfigInt("RobotPort"));
  const std::string ip = ConfigController::getConfigString("RobotIP");

  if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0)
  {
    std::cerr << "Invalid address / Address not supported\n";
    exit(EXIT_FAILURE);
  }

  if (connect(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0)
  {
    std::cerr << "Connection failed\n";
    exit(EXIT_FAILURE);
  }

  std::cout << "Connected using LinuxClient.\n";
}

void LinuxClient::sendCommand()
{
  while (keepRunning_)
  {
    std::unique_lock lock(mutex_);
    if (commandToSend_.empty())
    {
      lock.unlock();
      std::this_thread::sleep_for(std::chrono::milliseconds(30));
      continue;
    }

    bool atGoal = false;
    ssize_t bytesSent = send(serverSocket, commandToSend_.c_str(), commandToSend_.size(), 0);
    if (bytesSent < 0)
    {
      std::cerr << "Failed to send command\n";
    }
    else
    {
      Utility::appendToFile("log.txt", "\n//\n// Sent: " + commandToSend_ + "\n//\n");
      atGoal = commandToSend_.contains("out");
    }
    commandToSend_ = "";

    lock.unlock();
    char buffer[1024] = {0};
    ssize_t bytesReceived = recv(serverSocket, buffer, 1024, 0);
    if (bytesReceived > 0)
    {
      lock.lock();
      if (not firstRecieved_)
      {
        commandToSend_ = "f 500 -15 in\n";
        firstRecieved_ = true;
        continue;
      }
      commandToSend_ = "";

      if (atGoal)
      {
        MainController::completedGoalDelivery();
      }
      MainController::completedCommand();
      Utility::appendToFile("log.txt", "Received Response\n");
    }
    Utility::appendToFile("log.txt", "Done with sending\n");
  }
}

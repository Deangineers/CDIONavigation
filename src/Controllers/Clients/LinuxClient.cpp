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

#include "Utility/ConfigController.h"

LinuxClient::LinuxClient() : commandToSend_(""), keepRunning_(true)
{
  LinuxClient::connectToServer();
  LinuxClient::sendBallCollectionCommand("in\n");
  sendThread_ = std::thread(&LinuxClient::sendCommand, this);
}

LinuxClient::~LinuxClient()
{
  keepRunning_ = false;
}

void LinuxClient::sendCommandAndAddNewLine(const std::string& command)
{
  std::lock_guard lock(mutex_);
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
  if (serverSocket < 0) {
    std::cerr << "Failed to create socket\n";
    exit(EXIT_FAILURE);
  }

  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(ConfigController::getConfigInt("RobotPort"));
  const std::string ip = ConfigController::getConfigString("RobotIP");

  if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
    std::cerr << "Invalid address / Address not supported\n";
    exit(EXIT_FAILURE);
  }

  if (connect(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
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
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      continue;
    }
    if (not ballCollectionCommand_.empty())
    {
      ssize_t bytesSent = send(serverSocket, ballCollectionCommand_.c_str(), ballCollectionCommand_.size(), 0);
      if (bytesSent < 0) {
        std::cerr << "Failed to send command\n";
      } else {
        std::cout << "Sent: " << ballCollectionCommand_;
      }
      ballCollectionCommand_ = "";
    }


    ssize_t bytesSent = send(serverSocket, commandToSend_.c_str(), commandToSend_.size(), 0);
    if (bytesSent < 0) {
      std::cerr << "Failed to send command\n";
    } else {
      std::cout << "Sent: " << commandToSend_;
    }
    commandToSend_ = "";

    lock.unlock();
    char buffer[1024] = {0};
    ssize_t bytesReceived = recv(serverSocket, buffer, 1024, 0);
    if (bytesReceived > 0)
    {
      commandToSend_ = "";
    }
  }
}
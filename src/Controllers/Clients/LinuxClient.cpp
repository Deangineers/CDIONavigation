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

LinuxClient::LinuxClient()
{
  LinuxClient::connectToServer();
}

void LinuxClient::sendCommandAndAddNewLine(const std::string& command)
{
  sendCommand(command + "\n");
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

void LinuxClient::sendCommand(const std::string& command) const
{
  ssize_t bytesSent = send(serverSocket, command.c_str(), command.size(), 0);
  if (bytesSent < 0) {
    std::cerr << "Failed to send command\n";
  } else {
    std::cout << "Sent: " << command;
  }
}
//
// Created by Emil Damholt Savino on 01/05/2025.
//

#include "Client.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fstream>
#include <string>

constexpr std::string FILE_NAME = "keystore.txt";
constexpr int SERVER_PORT = 1234;

Client::Client() {
  connectToServer();
}

void Client::connectToServer() {
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket < 0) {
    std::cerr << "Failed to create socket\n";
    exit(EXIT_FAILURE);
  }

  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(SERVER_PORT);
  std::string ip = getServerIp();

  if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
    std::cerr << "Invalid address / Address not supported\n";
    exit(EXIT_FAILURE);
  }

  if (connect(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
    std::cerr << "Connection failed\n";
    exit(EXIT_FAILURE);
  }

  std::cout << "Connected to server.\n";
}

void Client::sendCommand(const std::string& command) const {
  ssize_t bytesSent = send(serverSocket, command.c_str(), command.size(), 0);
  if (bytesSent < 0) {
    std::cerr << "Failed to send command\n";
  } else {
    std::cout << "Sent: " << command << std::endl;
  }
}

std::string Client::getServerIp()
{
  std::ifstream inputFile(FILE_NAME);
  std::string ip;

  if (inputFile.is_open()) {
    std::getline(inputFile, ip);
    inputFile.close();
  } else {
    std::cerr << "Could not open file: " << FILE_NAME << std::endl;
  }

  return ip;
}


//
// Created by Emil Damholt Savino on 01/05/2025.
//

#include "Client.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

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

  if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
    std::cerr << "Invalid address / Address not supported\n";
    exit(EXIT_FAILURE);
  }

  if (connect(serverSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
    std::cerr << "Connection failed\n";
    exit(EXIT_FAILURE);
  }

  std::cout << "Connected to server.\n";
}

void Client::sendCommand(std::string& command) const {
  ssize_t bytesSent = send(serverSocket, command.c_str(), command.size(), 0);
  if (bytesSent < 0) {
    std::cerr << "Failed to send command\n";
  } else {
    std::cout << "Sent: " << command << std::endl;
  }
}


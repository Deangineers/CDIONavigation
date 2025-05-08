//
// Created by Emil Damholt Savino on 01/05/2025.
//

#include "Client.h"
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "ws2_32.lib")
  using socket_t = SOCKET;
  #define CLOSE_SOCKET closesocket
  #define INIT_NETWORK() initializeWinsock()
  #define CLEANUP_NETWORK() WSACleanup()
#else
  #include <unistd.h>
  #include <arpa/inet.h>
  #include <sys/socket.h>
  using socket_t = int;
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR -1
  #define CLOSE_SOCKET close
  #define INIT_NETWORK() ((void)0)
  #define CLEANUP_NETWORK() ((void)0)
#endif

constexpr const char* FILE_NAME = "keystore.txt";
constexpr int SERVER_PORT = 1234;

Client::Client() {
  INIT_NETWORK();
  connectToServer();
}

Client::~Client() {
  CLOSE_SOCKET(serverSocket);
  CLEANUP_NETWORK();
}

#ifdef _WIN32
void Client::initializeWinsock() {
  WSADATA wsaData;
  int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (result != 0) {
    std::cerr << "WSAStartup failed: " << result << std::endl;
    exit(EXIT_FAILURE);
  }
}
#endif

void Client::connectToServer() {
  serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == INVALID_SOCKET) {
    #ifdef _WIN32
      std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
    #else
      std::cerr << "Socket creation failed\n";
    #endif
    CLEANUP_NETWORK();
    exit(EXIT_FAILURE);
  }

  sockaddr_in serverAddr{};
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(SERVER_PORT);
  std::string ip = getServerIp();

  if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
    std::cerr << "Invalid address / Address not supported\n";
    CLOSE_SOCKET(serverSocket);
    CLEANUP_NETWORK();
    exit(EXIT_FAILURE);
  }

  if (connect(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
    #ifdef _WIN32
      std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
    #else
      std::cerr << "Connection failed\n";
    #endif
    CLOSE_SOCKET(serverSocket);
    CLEANUP_NETWORK();
    exit(EXIT_FAILURE);
  }

  std::cout << "Connected to server.\n";
}

void Client::sendCommand(const std::string& command) const {
  int bytesSent = send(serverSocket, command.c_str(), static_cast<int>(command.size()), 0);
  if (bytesSent == SOCKET_ERROR) {
    #ifdef _WIN32
      std::cerr << "Failed to send command: " << WSAGetLastError() << std::endl;
    #else
      std::cerr << "Failed to send command\n";
    #endif
  } else {
    std::cout << "Sent: " << command << std::endl;
  }
}

std::string Client::getServerIp() {
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

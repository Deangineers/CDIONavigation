//
// Created by eal on 6/3/25.
//

#include "MockClient.h"

#include "Utility/Utility.h"

MockClient::MockClient()
{
  MockClient::connectToServer();
}

void MockClient::sendCommandAndAddNewLine(const std::string& command)
{
  std::cout << command << "\n";
  Utility::appendToFile("log.txt", "\n//\n// Sent: " + command + "\n//\n");
}

void MockClient::sendBallCollectionCommand(const std::string& command)
{
  std::cout << command << "\n";
  Utility::appendToFile("log.txt", "\n//\n// Sent: " + command + "\n//\n");
}

void MockClient::connectToServer()
{
  std::cout << "Connected Using Mock Client\n";
}

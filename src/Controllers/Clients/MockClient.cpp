//
// Created by eal on 6/3/25.
//

#include "MockClient.h"

MockClient::MockClient()
{
  MockClient::connectToServer();
}

void MockClient::sendBallCollectionCommand(const std::string& command)
{
  std::cout << command << "\n";
}

void MockClient::sendMovementCommand(const std::string& command)
{
  std::cout << command << "\n";
}

void MockClient::connectToServer()
{
  std::cout << "Connected Using Mock Client\n";
}

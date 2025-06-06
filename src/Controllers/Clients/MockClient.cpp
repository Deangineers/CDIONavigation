//
// Created by eal on 6/3/25.
//

#include "MockClient.h"

MockClient::MockClient()
{
  MockClient::connectToServer();
}

void MockClient::sendCommand(const std::string& command) const
{
  std::cout << "Sent Command: " << command;
}

void MockClient::connectToServer()
{
  std::cout << "Connected Using Mock Client\n";
}

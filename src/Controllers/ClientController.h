//
// Created by Elias Aggergaard Larsen on 10/06/2025.
//

#ifndef CLIENTCONTROLLER_H
#define CLIENTCONTROLLER_H
#include <memory>
#include "../Models/Command.h"
#include "Clients/IClient.h"
#include <vector>


class ClientController
{
public:
  explicit ClientController(std::unique_ptr<IClient> client);
  void sendCommand(const Command& command);

private:
  void clearCommands();
  std::vector<Command> commands_;
  std::unique_ptr<IClient> client_;
};


#endif //CLIENTCONTROLLER_H

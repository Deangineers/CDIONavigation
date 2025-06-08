//
// Created by EAL on 02/05/2025.
//


#ifndef LINUXCLIENT_H
#define LINUXCLIENT_H
#include "IClient.h"

class LinuxClient : public IClient
{
public:
  LinuxClient();
  void sendCommandAndAddNewLine(const std::string& command) override;
private:
  void connectToServer() override;
  void sendCommand(const std::string& command) const;
};

#endif //LINUXCLIENT_H
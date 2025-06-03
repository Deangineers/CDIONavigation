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
  void sendCommand(const std::string& command) const override;
private:
  void connectToServer() override;
};

#endif //LINUXCLIENT_H
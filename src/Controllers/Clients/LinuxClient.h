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
  void sendBallCollectionCommand(const std::string& command) override;
  void sendMovementCommand(const std::string& command) override;
private:
  void connectToServer() override;
  void sendCommand(const std::string& command) const;
  std::string lastSentMovementCommand_ = "s\n";
  std::string lastSentBallCommand_ = "s c\n";
};

#endif //LINUXCLIENT_H
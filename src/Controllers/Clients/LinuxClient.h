//
// Created by EAL on 02/05/2025.
//


#ifndef LINUXCLIENT_H
#define LINUXCLIENT_H
#include <thread>

#include "IClient.h"

class LinuxClient : public IClient
{
public:
  LinuxClient();
  ~LinuxClient() override;
  void sendCommandAndAddNewLine(const std::string& command) override;
  void sendBallCollectionCommand(const std::string& command) override;
private:
  void connectToServer() override;
  void sendCommand();
  std::string commandToSend_;
  std::string ballCollectionCommand_;
  std::mutex mutex_;
  std::thread sendThread_;
  bool keepRunning_;
};

#endif //LINUXCLIENT_H
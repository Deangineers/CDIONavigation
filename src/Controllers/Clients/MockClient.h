//
// Created by eal on 6/3/25.
//

#ifndef MOCKCLIENT_H
#define MOCKCLIENT_H
#include "IClient.h"


class MockClient : public IClient
{
public:
  MockClient();
  void sendCommandAndAddNewLine(const std::string& command) override;
  void sendBallCollectionCommand(const std::string& command) override;
private:
  void connectToServer() override;
};



#endif //MOCKCLIENT_H

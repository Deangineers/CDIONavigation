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
  void sendCommand(const std::string& command) const override;
  void connectToServer() override;
};



#endif //MOCKCLIENT_H

//
// Created by eal on 6/3/25.
//

#ifndef ICLIENT_H
#define ICLIENT_H
#include <fstream>
#include <iostream>
#include <string>


class IClient
{
public:
  virtual ~IClient() = default;
  virtual void sendCommandAndAddNewLine(const std::string& command) = 0;
  virtual void sendBallCollectionCommand(const std::string& command) = 0;
  virtual void connectToServer() = 0;
protected:
  int serverSocket = 0;
};



#endif //ICLIENT_H

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
  virtual void sendCommand(const std::string& command) const = 0;
  virtual void connectToServer() = 0;
  static std::string getServerIp()
  {
    std::ifstream inputFile("keystore.txt");
    std::string ip;

    if (inputFile.is_open())
    {
      std::getline(inputFile, ip);
      inputFile.close();
    } else
    {
      std::cerr << "Could not open file: keystore.txt" << std::endl;
    }

    return ip;
  }
protected:
  int serverSocket = 0;
};



#endif //ICLIENT_H

//
// Created by Emil Damholt Savino on 01/05/2025.
//

#ifndef CLIENT_H
#define CLIENT_H
#include <string>


class Client {
public:
  Client();
  void sendCommand(const std::string& command) const;

private:
  int serverSocket{};
  void connectToServer();
  static std::string getServerIp();
};



#endif //CLIENT_H

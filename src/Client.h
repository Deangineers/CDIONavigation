//
// Created by Emil Damholt Savino on 01/05/2025.
//

#ifndef CLIENT_H
#define CLIENT_H
#include <string>


class Client {
public:
  Client();
  void sendCommand(std::string& command) const;

private:
  int serverSocket{};
  void connectToServer();
};



#endif //CLIENT_H

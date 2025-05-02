//
// Created by Emil Damholt Savino on 01/05/2025.
//

#ifndef COMMAND_H
#define COMMAND_H
#include <string>


class Command {
public:
  Command();
  Command(const std::string& action, int speed, const std::string& motor);
  void setAction(const std::string& action);
  void setSpeed(int speed);
  void setMotor(const std::string& motor);
  [[nodiscard]] std::string formatToSend() const;

private:
  std::string action;
  int speed;
  std::string motor;
};



#endif //COMMAND_H

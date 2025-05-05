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

  std::string& getAction();
  [[nodiscard]] int getSpeed() const;
  std::string& getMotor();

private:
  std::string action_;
  int speed_;
  std::string motor_;
};



#endif //COMMAND_H

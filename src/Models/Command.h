//
// Created by Emil Damholt Savino on 01/05/2025.
//

#ifndef COMMAND_H
#define COMMAND_H
#include <string>


class Command
{
public:
  Command();
  Command(const std::string& action, int speed, double distanceOrAngle, bool ballCollection);
  void setAction(const std::string& action);
  void setSpeed(int speed);
  void setDistanceOrAngle(double distanceOrAngle);
  void setBallCollection(bool ballCollectionGrabsBalls);
  [[nodiscard]] std::string formatToSend() const;

  [[nodiscard]] std::string getAction() const;
  [[nodiscard]] int getSpeed() const;
  [[nodiscard]] double getDistanceOrAngle() const;
  [[nodiscard]] bool getBallCollectionGrabsBalls() const;

private:
  std::string action_;
  int speed_;
  double distanceOrAngle_;
  bool ballCollectionGrabsBalls_;
};


#endif //COMMAND_H

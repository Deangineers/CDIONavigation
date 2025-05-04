//
// Created by eal on 01/05/25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include "Client.h"
#include "NavigationController.h"
#include "../Models/Command.h"


class MainController
{
public:
  static void init();
  static NavigationController* getNavController();
  static void navigateAndSendCommand();
  static Command handleBallCollectionMotor(const JourneyModel* journey);
  static Command journeyToCommand(const JourneyModel* journey);
private:
  static inline std::unique_ptr<NavigationController> navigationController_ = nullptr;
  static inline std::unique_ptr<Client> client_ = nullptr;
  static inline void updateBallCollectionAction(const std::string& action);
};



#endif //MAINCONTROLLER_H

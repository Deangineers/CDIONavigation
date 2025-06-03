//
// Created by eal on 01/05/25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include "NavigationController.h"
#include "../Models/Command.h"
#include "Clients/IClient.h"


class MainController
{
public:
  static void init();
  static void mockInit();
  static NavigationController* getNavController();
  static void navigateAndSendCommand();
  static Command handleBallCollectionMotor(const JourneyModel* journey);
  static Command journeyToCommand(const JourneyModel* journey);
private:
  static inline std::unique_ptr<NavigationController> navigationController_ = nullptr;
  static inline std::unique_ptr<IClient> client_ = nullptr;
  static inline void updateBallCollectionAction(const std::string& action);
};



#endif //MAINCONTROLLER_H

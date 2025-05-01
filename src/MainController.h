//
// Created by eal on 01/05/25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include "Client.h"
#include "NavigationController.h"


class MainController
{
public:
  static void init();
  static NavigationController* getNavController();
  static void navigateAndSendCommand();
private:
  static inline std::unique_ptr<NavigationController> navigationController_ = nullptr;
  static inline std::unique_ptr<Client> client_ = nullptr;
};



#endif //MAINCONTROLLER_H

//
// Created by eal on 01/05/25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include "NavigationController.h"


class MainController
{
public:
  static void init();
  static NavigationController* getNavController();
private:
  static inline std::unique_ptr<NavigationController> navigationController_ = nullptr;
};



#endif //MAINCONTROLLER_H

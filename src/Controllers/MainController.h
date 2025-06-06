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
  static void addCourseObject(std::unique_ptr<CourseObject>&& courseObject);
  static const char* navigateAndSendCommand();
  static Command handleBallCollectionMotor(const JourneyModel* journey);
  static Command journeyToCommand(const JourneyModel* journey);
private:
  static inline std::unique_ptr<NavigationController> navigationController_ = nullptr;
  static inline bool isFirst_ = true;
  static inline std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
};



#endif //MAINCONTROLLER_H

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
  static void navigateAndSendCommand();
  static Command handleBallCollectionMotor(const JourneyModel* journey);
  static std::string journeyToCommand(const JourneyModel* journey);
  static int findMaxValue(const int* cords, int size, int maxValueAllowed);
  static int findMinValue(const int* cords, int size, int minValueAllowed);
private:
  static inline std::unique_ptr<NavigationController> navigationController_ = nullptr;
  static inline std::unique_ptr<IClient> client_ = nullptr;
};



#endif //MAINCONTROLLER_H

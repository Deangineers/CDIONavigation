//
// Created by eal on 01/05/25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include <opencv2/core/types.hpp>

#include "ClientController.h"
#include "NavigationController.h"
#include "../Models/Command.h"
#include "Clients/IClient.h"


class MainController
{
public:
  static void init();
  static void mockInit();
  static void addCourseObject(std::unique_ptr<CourseObject>&& courseObject);
  static void addBlockedObjects(const std::vector<cv::Point>& blockedObjects);
  static void navigateAndSendCommand();
  static Command journeyToCommand(const JourneyModel* journey);

private:
  static int findMaxValue(const int* cords, int size, int maxValueAllowed);
  static int findMinValue(const int* cords, int size, int minValueAllowed);

private:
  static inline std::unique_ptr<NavigationController> navigationController_ = nullptr;
  static inline std::unique_ptr<ClientController> clientController_ = nullptr;
};


#endif //MAINCONTROLLER_H

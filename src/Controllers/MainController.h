//
// Created by eal on 01/05/25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include <opencv2/core/types.hpp>

#include "ClientController.h"
#include "NavigationController.h"
#include "../Models/BlockingObject.h"
#include "../Models/Command.h"
#include "Clients/IClient.h"


class MainController
{
public:
  static void init();
  static void mockInit();
  static void addCourseObject(std::unique_ptr<CourseObject>&& courseObject);
  static void addBlockedObject(std::unique_ptr<BlockingObject> blockingObject);
  static void navigateAndSendCommand(cv::Mat* frame);
  static Command journeyToCommand(const JourneyModel* journey);
  static cv::Mat* getFrame();

private:
  static int findMaxValue(const int* cords, int size, int maxValueAllowed);
  static int findMinValue(const int* cords, int size, int minValueAllowed);

private:
  static inline std::unique_ptr<NavigationController> navigationController_ = nullptr;
  static inline std::unique_ptr<ClientController> clientController_ = nullptr;
  static inline cv::Mat* frame_;
};


#endif //MAINCONTROLLER_H

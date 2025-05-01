#include <iostream>

#include "NavigationController.h"

extern "C"
{
  void createObject(int x1, int y1, int x2, int y2, const char* label)
  {
    NavigationController::addCourseObject(std::move(std::make_unique<CourseObject>(x1,y1,x2,y2,label)));
  }

  void start()
  {
    NavigationController::navigate();
    NavigationController::clearObjects();
  }

}

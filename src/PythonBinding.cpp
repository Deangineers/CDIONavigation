#include <future>
#include <iostream>

#include "Controllers/MainController.h"
#include "Controllers/NavigationController.h"

extern "C"
{

  void init()
  {
    MainController::init();
  }

  void createObject(int x1, int y1, int x2, int y2, const char* label)
  {
    MainController::getNavController()->addCourseObject(std::move(std::make_unique<CourseObject>(x1,y1,x2,y2,label)));
  }

  void navigateAndSendCommand()
  {
    // Optimal but risky:
    //auto thread = std::thread(MainController::navigateAndSendCommand);
    //thread.detach();

    MainController::navigateAndSendCommand();
  }

}

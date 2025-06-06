#include "Controllers/MainController.h"
#include "Controllers/NavigationController.h"

extern "C"
{

  void init()
  {
    MainController::init();
  }

  void mockinit()
  {
    MainController::mockInit();
  }

  void createObject(int x1, int y1, int x2, int y2, const char* label)
  {
    MainController::addCourseObject(std::move(std::make_unique<CourseObject>(x1,y1,x2,y2,label)));
  }

  void navigateAndSendCommand()
  {
    // Optimal but risky:
    //auto thread = std::thread(MainController::navigateAndSendCommand);
    //thread.detach();
    MainController::navigateAndSendCommand();
  }

  void createBlockedObjects(int* x, int* y, int size)
  {
    int maxX = MainController::findMaxValue(x, size,INT32_MAX);
    int maxY = MainController::findMaxValue(y, size,INT32_MAX);
    int minX = MainController::findMinValue(x, size,INT32_MIN);
    int minY = MainController::findMinValue(y, size,INT32_MIN);

    int wallWidth = ConfigController::getConfigInt("WallWidth");
    int distanceToOffsetForCross = ConfigController::getConfigInt("OuterWallOffsetWhenSearchingForCross");

    //
    // Create Surrounding walls
    //

    // Top Wall
    createObject(minX,minY,maxX,minY + wallWidth,"blockedObject");
    // Left Wall
    createObject(minX,minY,minX + wallWidth,maxY,"blockedObject");
    // Right Wall
    createObject(maxX - wallWidth,minY,maxX,maxY,"blockedObject");
    // Bottom Wall
    createObject(minX,maxY - wallWidth,maxX,maxY,"blockedObject");


    //
    // Create Cross
    //
    std::vector<std::pair<int,int>> vector;
    vector.reserve(16384);
    for (int i = 0; i < size; i++)
    {
      int xValue = x[i];
      int yValue = y[i];
      if (xValue < maxX - distanceToOffsetForCross && minX + distanceToOffsetForCross < xValue)
      {
        if (yValue < maxY - distanceToOffsetForCross && minY + distanceToOffsetForCross < yValue)
        {
          createObject(xValue,yValue,xValue,yValue,"blockedObject");
        }
      }
    }


  }

}

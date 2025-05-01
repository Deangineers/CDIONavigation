#include <iostream>
extern "C"
{
  void createObject(int x1, int y1, int x2, int y2, const char* label)
  {
    std::cout << "Drawing line from (" << x1 << ", " << y1 << ") to (" << x2 << ", " << y2 << ") with label: " << label << std::endl;
  }

  void start()
  {
    std::cout << "Starting line drawing system..." << std::endl;
  }

}

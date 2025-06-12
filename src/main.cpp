//
// Created by Elias Aggergaard Larsen on 12/06/2025.
//

#include "Controllers/MainController.h"
#include <opencv2/opencv.hpp>

int main()
{
  cv::VideoCapture cap(0);
  if (!cap.isOpened())
  {
    std::cerr << "Failed to open webcam\n";
    return -1;
  }
  cv::Mat frame;
  while (cap.read(frame))
  {
    cv::imshow("frame", frame);

    if (cv::waitKey(1) == 27) // Wait for 1 ms and break on 'Esc' key
      break;
  }

  cap.release();
  cv::destroyAllWindows();
  return 0;
}

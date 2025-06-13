//
// Created by Elias Aggergaard Larsen on 12/06/2025.
//

#include "Controllers/MainController.h"
#include <opencv2/opencv.hpp>

#include "Controllers/ImageProcessorController.h"

int main()
{
  MainController::mockInit();
  cv::VideoCapture cap(2);

  if (!cap.isOpened())
  {
    std::cerr << "Failed to open webcam\n";
    return -1;
  }
  cap.set(cv::CAP_PROP_FRAME_WIDTH,  1920);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

  cv::Mat frame;
  ImageProcessorController processor;


  while (cap.read(frame))
  {
    // Apply red pixel detection
    processor.detectRedPixels(frame);
    processor.detectBalls(frame);

    // Convert to HSV for color-based detection
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    // Find and create robot front (green-ish)
    processor.findAndCreate(
      frame,
      hsv,
      cv::Scalar(35, 50, 50), // Lower HSV bound for green
      cv::Scalar(85, 255, 255), // Upper HSV bound for green
      "robotFront"
    );

    // Find and create robot back (blue-purple)
    processor.findAndCreate(
      frame,
      hsv,
      cv::Scalar(125, 80, 80), // Lower HSV bound for purple
      cv::Scalar(155, 255, 255), // Upper HSV bound for purple
      "robotBack"
    );
    cv::imshow("EngineBase", frame);
    MainController::navigateAndSendCommand();
    if (cv::waitKey(33) == 27) // Wait for 1 ms and break on 'Esc' key
    {
      break;
    }
  }

  cap.release();
  cv::destroyAllWindows();
  return 0;
}

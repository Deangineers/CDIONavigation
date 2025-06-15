//
// Created by Elias Aggergaard Larsen on 12/06/2025.
//

#include "Controllers/MainController.h"
#include <opencv2/opencv.hpp>

#include "Controllers/ImageProcessing/CloudyImageProcessor.h"
#include "Controllers/ImageProcessing/ImageProcessor.h"

int main()
{
  MainController::mockInit();
  cv::VideoCapture cap(0);

  if (!cap.isOpened())
  {
    std::cerr << "Failed to open webcam\n";
    return -1;
  }
  cap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

  cv::Mat frame;
  std::unique_ptr<ImageProcessor> processor = std::make_unique<CloudyImageProcessor>();

  while (cap.read(frame))
  {
    frame = cv::imread("../../TestImages/img3.jpg", cv::IMREAD_COLOR_BGR);
    processor->processImage(frame);

    MainController::navigateAndSendCommand(&frame);
    cv::imshow("EngineBase", frame);
    if (cv::waitKey(33) == 27) // Wait for 1 ms and break on 'Esc' key
    {
      break;
    }
  }

  cap.release();
  cv::destroyAllWindows();
  return 0;
}

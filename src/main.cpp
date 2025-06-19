//
// Created by Elias Aggergaard Larsen on 12/06/2025.
//

#include "Controllers/MainController.h"
#include <opencv2/opencv.hpp>

#include "Controllers/ImageProcessing/CloudyImageProcessor.h"
#include "Controllers/ImageProcessing/ImageProcessor.h"

int main()
{
  cv::VideoCapture cap;
#if __linux__
  MainController::init();
  cap = cv::VideoCapture(2);
#else
  MainController::mockInit();
  cap = cv::VideoCapture(0);
#endif

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
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    // frame = cv::imread("../../TestImages/ballsInCornerWhite.jpeg", cv::IMREAD_COLOR);
    processor->processImage(frame);

    MainController::navigateAndSendCommand(&frame);

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    cv::putText(frame, "FPS: " + std::to_string(1000 / duration.count()),
                {1700, 1000}, cv::FONT_HERSHEY_SIMPLEX,
                1, cv::Scalar(255, 255, 0), 2);

    cv::imshow("EngineBase", frame);

    if (cv::waitKey(1) == 27) // Wait for 1 ms and break on 'Esc' key
    {
      return 0;
    }
  }

  cap.release();
  cv::destroyAllWindows();
  return 0;
}

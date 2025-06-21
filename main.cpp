//
// Created by Elias Aggergaard Larsen on 12/06/2025.
//

#include "src/Controllers/MainController.h"
#include <opencv2/opencv.hpp>

#include "src/Controllers/ImageProcessing/CloudyImageProcessor.h"
#include "src/Controllers/ImageProcessing/ImageProcessor.h"

int main()
{
  cv::VideoCapture cap;
#if __linux__
  MainController::mockInit();
  cap = cv::VideoCapture("/dev/video4",cv::CAP_V4L2);
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
  cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));
  cap.set(cv::CAP_PROP_FPS, 30);

  cv::Mat frame;
  auto ballProcessor = std::make_shared<BallProcessor>();
  const auto processor = std::make_unique<CloudyImageProcessor>(ballProcessor);

  double lastFPS = 0;

  std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
  while (cap.read(frame))
  {
    //cv::imwrite("../../TestImages/newTestImages0.jpeg", frame);
    //frame = cv::imread("../TestImages/testImage5.jpg", cv::IMREAD_COLOR);


    processor->processImage(frame);

    MainController::navigateAndSendCommand(&frame);

    cv::putText(frame, "FPS: " + std::to_string(lastFPS),
                {1700, 1000}, cv::FONT_HERSHEY_SIMPLEX,
                1, cv::Scalar(255, 255, 0), 2);
    cv::imshow("EngineBase", frame);

    if (cv::waitKey(1) == 27) // Wait for 1 ms and break on 'Esc' key
    {
      return 0;
    }
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    lastFPS = 1000.0 / duration.count();
    start = std::chrono::high_resolution_clock::now();
    cap.grab();
  }

  cap.release();
  cv::destroyAllWindows();
  return 0;
}

//
// Created by Elias Aggergaard Larsen on 19/06-2025.
//

#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/mat.hpp>

#include "TestBallProcessor.h"
#include "../src/Controllers/MainController.h"
#include "../src/Controllers/ImageProcessing/CloudyImageProcessor.h"
#include "gtest/gtest.h"
#include "Utility/ConfigController.h"

class BallDetectionTests : public testing::Test
{
public:
  BallDetectionTests()
  {
    MainController::testInit();
    ballProcessor = std::make_shared<TestBallProcessor>();
    imageProcessor = std::make_shared<CloudyImageProcessor>(ballProcessor);
  }

  std::shared_ptr<CloudyImageProcessor> imageProcessor;
  std::shared_ptr<TestBallProcessor> ballProcessor;
};

TEST_F(BallDetectionTests, ballDetectionTest1)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage1.jpg");
  imageProcessor->processImage(frame);

  EXPECT_EQ(ballProcessor->getBallCounter(), 0);
}

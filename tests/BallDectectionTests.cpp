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
    ConfigController::TESTsetConfigInt("ImagesToAverage", 1);
    ConfigController::TESTsetConfigInt("AmountOfSeenBeforeCreate", 0);
  }

  std::shared_ptr<CloudyImageProcessor> imageProcessor;
  std::shared_ptr<TestBallProcessor> ballProcessor;
};

int totalDiff = 0;

TEST_F(BallDetectionTests, ballsInCorner)
{
  cv::Mat frame = cv::imread("../../TestImages/ballsInCorner.jpg");
  imageProcessor->processImage(frame);

  int expectedBalls = 5;
  EXPECT_EQ(ballProcessor->getBallCounter(), expectedBalls);
}

TEST_F(BallDetectionTests, ballsInCornerWhite)
{
  cv::Mat frame = cv::imread("../../TestImages/ballsInCornerWhite.jpg");
  imageProcessor->processImage(frame);

  int expectedBalls = 7;
  int seenBalls = ballProcessor->getBallCounter();
  totalDiff += std::abs(expectedBalls - seenBalls);
  EXPECT_EQ(seenBalls, expectedBalls);
}


TEST_F(BallDetectionTests, testImage1)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage1.jpg");
  imageProcessor->processImage(frame);

  int expectedBalls = 8;
  int seenBalls = ballProcessor->getBallCounter();
  totalDiff += std::abs(expectedBalls - seenBalls);
  EXPECT_EQ(seenBalls, expectedBalls);
}

TEST_F(BallDetectionTests, testImage2)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage2.jpg");
  imageProcessor->processImage(frame);

  int expectedBalls = 11;
  int seenBalls = ballProcessor->getBallCounter();
  totalDiff += std::abs(expectedBalls - seenBalls);
  EXPECT_EQ(seenBalls, expectedBalls);
}

TEST_F(BallDetectionTests, testImage3)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage3.jpg");
  imageProcessor->processImage(frame);

  int expectedBalls = 10;
  int seenBalls = ballProcessor->getBallCounter();
  totalDiff += std::abs(expectedBalls - seenBalls);
  EXPECT_EQ(seenBalls, expectedBalls);
}

TEST_F(BallDetectionTests, testImage4)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage4.jpg");
  imageProcessor->processImage(frame);

  int expectedBalls = 11;
  int seenBalls = ballProcessor->getBallCounter();
  totalDiff += std::abs(expectedBalls - seenBalls);
  EXPECT_EQ(seenBalls, expectedBalls);
}

TEST_F(BallDetectionTests, testImage5)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage5.jpg");
  imageProcessor->processImage(frame);

  int expectedBalls = 10;
  int seenBalls = ballProcessor->getBallCounter();
  totalDiff += std::abs(expectedBalls - seenBalls);
  EXPECT_EQ(seenBalls, expectedBalls);
}

TEST_F(BallDetectionTests, totalDiff)
{
  EXPECT_EQ(totalDiff, 0);
}

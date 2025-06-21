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


int totalDiff = 0;
int aggregateDiff = 0;
int amountOverTarget = 0;
int amountUnderTarget = 0;

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

  void handleExpect(int expected)
  {
    int seenBalls = ballProcessor->getBallCounter();
    int diff = seenBalls - expected;
    totalDiff += std::abs(diff);
    aggregateDiff += diff;
    if (diff > 0)
    {
      amountOverTarget++;
    }
    else if (diff < 0)
    {
      amountUnderTarget++;
    }
    EXPECT_EQ(seenBalls, expected);
  }

  std::shared_ptr<CloudyImageProcessor> imageProcessor;
  std::shared_ptr<TestBallProcessor> ballProcessor;
};


TEST_F(BallDetectionTests, ballsInCorner)
{
  cv::Mat frame = cv::imread("../../TestImages/ballsInCorner.jpg");
  imageProcessor->processImage(frame);
  handleExpect(5);
}

TEST_F(BallDetectionTests, ballsInCornerWhite)
{
  cv::Mat frame = cv::imread("../../TestImages/ballsInCornerWhite.jpg");
  imageProcessor->processImage(frame);
  handleExpect(7);
}


TEST_F(BallDetectionTests, testImage1)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage1.jpg");
  imageProcessor->processImage(frame);
  handleExpect(8);
}

TEST_F(BallDetectionTests, testImage2)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage2.jpg");
  imageProcessor->processImage(frame);
  handleExpect(11);
}

TEST_F(BallDetectionTests, testImage3)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage3.jpg");
  imageProcessor->processImage(frame);
  handleExpect(10);
}

TEST_F(BallDetectionTests, testImage4)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage4.jpg");
  imageProcessor->processImage(frame);
  handleExpect(11);
}

TEST_F(BallDetectionTests, testImage5)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage5.jpg");
  imageProcessor->processImage(frame);
  handleExpect(10);
}

TEST_F(BallDetectionTests, testImage6)
{
  cv::Mat frame = cv::imread("../TestImages/testImage6.jpg");
  imageProcessor->processImage(frame);
  handleExpect(7);
}

TEST_F(BallDetectionTests, testImage7)
{
  cv::Mat frame = cv::imread("../TestImages/testImage7.jpg");
  imageProcessor->processImage(frame);
  handleExpect(7);
}

TEST_F(BallDetectionTests, testImage8)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage8.jpg");
  imageProcessor->processImage(frame);
  handleExpect(6);
}

TEST_F(BallDetectionTests, totalDiff)
{
  EXPECT_EQ(amountOverTarget, 0);
  EXPECT_EQ(amountUnderTarget, 0);
  EXPECT_EQ(totalDiff, 0);
  EXPECT_EQ(aggregateDiff, 0);
}

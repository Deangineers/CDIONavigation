//
// Created by localadmin on 21/06/2025.
//

#include "../cmake-build-release/_deps/googletest-src/googletest/include/gtest/gtest.h"
#include "../Simulator/TextureLocations.h"
#include "../src/Controllers/ObjectCounter.h"
#include "../src/Controllers/ImageProcessing/CloudyImageProcessor.h"
#include "../src/Controllers/MainController.h"

class FrontBackDetectionTests : public testing::Test
{
public:
  FrontBackDetectionTests()
  {
    MainController::testInit();
    imageProcessor = std::make_shared<CloudyImageProcessor>();
  }

  void handleExpect()
  {
    int frontSeenCounter = ObjectCounter::getCount("robotFront");
    int backSeenCounter = ObjectCounter::getCount("robotBack");
    EXPECT_EQ(frontSeenCounter,1);
    EXPECT_EQ(backSeenCounter,1);
    ObjectCounter::reset();
  }

  std::shared_ptr<CloudyImageProcessor> imageProcessor;
};

TEST_F(FrontBackDetectionTests, testImage1)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage1.jpg");
  imageProcessor->processImage(frame);
  handleExpect();
}

TEST_F(FrontBackDetectionTests, testImage2)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage2.jpg");
  imageProcessor->processImage(frame);
  handleExpect();
}

TEST_F(FrontBackDetectionTests, testImage3)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage3.jpg");
  imageProcessor->processImage(frame);
  handleExpect();
}

TEST_F(FrontBackDetectionTests, testImage4)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage4.jpg");
  imageProcessor->processImage(frame);
  handleExpect();
}

TEST_F(FrontBackDetectionTests, testImage5)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage5.jpg");
  imageProcessor->processImage(frame);
  handleExpect();
}

TEST_F(FrontBackDetectionTests, testImage6)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage6.jpg");
  imageProcessor->processImage(frame);
  handleExpect();
}

TEST_F(FrontBackDetectionTests, testImage7)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage7.jpg");
  imageProcessor->processImage(frame);
  handleExpect();
}

TEST_F(FrontBackDetectionTests, testImage8)
{
  cv::Mat frame = cv::imread("../../TestImages/testImage8.jpg");
  imageProcessor->processImage(frame);
  handleExpect();
}
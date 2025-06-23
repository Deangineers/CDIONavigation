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

TEST_F(FrontBackDetectionTests, testImages)
{
  int amountOfImages = 25;
  for (int i = 1; i <= amountOfImages; i++)
  {
    cv::Mat frame = cv::imread("../../TestImages/testImage"+std::to_string(i)+".jpg");
    imageProcessor->processImage(frame);
    handleExpect();
  }
}

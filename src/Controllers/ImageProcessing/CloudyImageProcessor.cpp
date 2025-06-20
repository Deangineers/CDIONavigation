//
// Created by Elias Aggergaard Larsen on 15/06/2025.
//

#include "CloudyImageProcessor.h"

#include "Utility/ConfigController.h"
#include "../MainController.h"


CloudyImageProcessor::CloudyImageProcessor(const std::shared_ptr<IBallProcessor>& ballProcessor) : ImageProcessor(
  ballProcessor)
{
}

void CloudyImageProcessor::detectRedPixels(const cv::Mat& frame)
{
  cv::Mat mask1, mask2, redMask;
  cv::inRange(hsv_, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), mask1);
  cv::inRange(hsv_, cv::Scalar(160, 100, 100), cv::Scalar(180, 255, 255), mask2);
  cv::bitwise_or(mask1, mask2, redMask);

  cv::Mat crossMask1, crossMask2, crossMask;
  cv::inRange(hsv_, cv::Scalar(0, 100, 100), cv::Scalar(5, 255, 255), crossMask1);
  cv::inRange(hsv_, cv::Scalar(170, 100, 100), cv::Scalar(180, 255, 255), crossMask2);
  cv::bitwise_or(crossMask1, crossMask2, crossMask);

  wallProcessor_->begin();
  redPixelHelperFunction(frame, redMask);
  crossHelperFunction(frame, crossMask);
}

void CloudyImageProcessor::detectBalls(const cv::Mat& frame)
{
  ballHelperFunction(frame, "orange");
}

void CloudyImageProcessor::detectEgg(const cv::Mat& frame)
{
  cv::Mat eggMask;
  cv::inRange(hsv_, cv::Scalar(0, 0, 200), cv::Scalar(180, 40, 255), eggMask);

  cv::morphologyEx(eggMask, eggMask, cv::MORPH_OPEN,
                   cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));

  eggHelperFunction(frame, eggMask);
}

void CloudyImageProcessor::detectFrontAndBack(const cv::Mat& frame)
{
  cv::Mat frontMask, backMask, finalMask;

  cv::inRange(hsv_, cv::Scalar(35, 50, 50), cv::Scalar(85, 255, 255), frontMask);
  cv::inRange(hsv_, cv::Scalar(120, 85, 85), cv::Scalar(170, 255, 255), backMask);
  frontAndBackHelperFunction(frame, frontMask, "robotFront");
  frontAndBackHelperFunction(frame, backMask, "robotBack");
}

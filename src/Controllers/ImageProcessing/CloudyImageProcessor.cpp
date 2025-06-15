//
// Created by Elias Aggergaard Larsen on 15/06/2025.
//

#include "CloudyImageProcessor.h"


void CloudyImageProcessor::detectRedPixels(const cv::Mat& frame)
{
  cv::Mat mask1, mask2, redMask;
  cv::inRange(hsv_, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), mask1);
  cv::inRange(hsv_, cv::Scalar(160, 100, 100), cv::Scalar(180, 255, 255), mask2);
  cv::bitwise_or(mask1, mask2, redMask);

  redPixelHelperFunction(frame, redMask);
}

void CloudyImageProcessor::detectBalls(const cv::Mat& frame)
{
  cv::Mat orangeMask, whiteMask, mask;
  cv::inRange(hsv_, cv::Scalar(5, 50, 50), cv::Scalar(30, 255, 255), orangeMask);
  cv::inRange(hsv_, cv::Scalar(0, 0, 200), cv::Scalar(180, 40, 255), whiteMask);

  cv::bitwise_or(orangeMask, whiteMask, mask);

  ballHelperFunction(frame, mask);
}

void CloudyImageProcessor::detectFrontAndBack(const cv::Mat& frame)
{
  cv::Mat frontMask, backMask, finalMask;

  cv::inRange(hsv_, cv::Scalar(35, 50, 50), cv::Scalar(85, 255, 255), frontMask);
  cv::inRange(hsv_, cv::Scalar(125, 80, 80), cv::Scalar(155, 255, 255), backMask);

  frontAndBackHelperFunction(frame, frontMask, "robotFront");
  frontAndBackHelperFunction(frame, backMask, "robotBack");
}

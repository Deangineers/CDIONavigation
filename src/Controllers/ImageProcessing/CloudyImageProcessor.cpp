//
// Created by Elias Aggergaard Larsen on 15/06/2025.
//

#include "CloudyImageProcessor.h"

#include "Utility/ConfigController.h"
#include "../MainController.h"


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
  cv::Mat totalMask;

  cv::Mat darkestOrangeMask, darkOrangeMask, orangeMask, lightOrangeMask, lightestOrangeMask;
  cv::Mat darkestWhiteMask, darkWhiteMask, whiteMask, lightWhiteMask, lightestWhiteMask;

  cv::inRange(hsv_, cv::Scalar(5, 150, 30),   cv::Scalar(10, 255, 130),  darkestOrangeMask);
  cv::inRange(hsv_, cv::Scalar(5, 150, 80),   cv::Scalar(15, 255, 180),  darkOrangeMask);
  cv::inRange(hsv_, cv::Scalar(10, 100, 150), cv::Scalar(20, 255, 255),  orangeMask);
  cv::inRange(hsv_, cv::Scalar(10, 80, 180),  cv::Scalar(25, 200, 255),  lightOrangeMask);
  cv::inRange(hsv_, cv::Scalar(10, 50, 200),  cv::Scalar(30, 150, 255),  lightestOrangeMask);

  cv::inRange(hsv_, cv::Scalar(0, 0, 80), cv::Scalar(179, 40, 140),  darkestWhiteMask);
  cv::inRange(hsv_, cv::Scalar(0, 0, 150),  cv::Scalar(179, 30, 180),  darkWhiteMask);
  cv::inRange(hsv_, cv::Scalar(0, 0, 180),  cv::Scalar(179, 25, 220),  whiteMask);
  cv::inRange(hsv_, cv::Scalar(0, 0, 220),  cv::Scalar(179, 20, 240),  lightWhiteMask);
  cv::inRange(hsv_, cv::Scalar(0, 0, 240),  cv::Scalar(179, 20, 255),  lightestWhiteMask);

  cv::Mat orangeTotalMask = darkestOrangeMask | darkOrangeMask | orangeMask | lightOrangeMask | lightestOrangeMask;
  cv::Mat whiteTotalMask = darkestWhiteMask | darkWhiteMask | whiteMask | lightWhiteMask | lightestWhiteMask;

  //cv::bitwise_or(orangeTotalMask,whiteTotalMask, totalMask);
  ballHelperFunction(frame, orangeTotalMask, "orange");
  ballHelperFunction(frame, whiteTotalMask, "white");

  //ballHelperFunction(frame, totalMask, "ball");
  //ballHelperFunction(frame, whiteMask, "white");

  //ballHelperFunction(frame, orangeMask, "o1");
  //ballHelperFunction(frame, darkOrangeMask, "o2");
  //ballHelperFunction(frame, darkestOrangeMask, "o3");
  //ballHelperFunction(frame, lightOrangeMask, "o4");
  //ballHelperFunction(frame, lightestOrangeMask, "o5");

  //ballHelperFunction(frame, darkestWhiteMask, "w1");
  //ballHelperFunction(frame, darkWhiteMask, "w2");
  //ballHelperFunction(frame, whiteMask, "w3");
  //ballHelperFunction(frame, lightWhiteMask, "w4");
  //ballHelperFunction(frame, lightestWhiteMask, "w5");

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

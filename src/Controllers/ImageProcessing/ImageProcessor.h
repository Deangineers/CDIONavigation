//
// Created by Elias Aggergaard Larsen on 15/06/2025.
//

#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H
#include <opencv2/opencv.hpp>

#include "BallProcessor.h"
#include "WallProcessor.h"

class ImageProcessor
{
public:
  explicit ImageProcessor(const std::shared_ptr<IBallProcessor>& ballProcessor);
  virtual ~ImageProcessor() = default;

  void processImage(const cv::Mat& frame);

protected:
  virtual void detectRedPixels(const cv::Mat& frame) = 0;
  virtual void detectBalls(const cv::Mat& frame) = 0;
  virtual void detectEgg(const cv::Mat& frame) = 0;
  virtual void detectFrontAndBack(const cv::Mat& frame) = 0;
  void redPixelHelperFunction(const cv::Mat& frame, cv::Mat& mask);
  void crossHelperFunction(const cv::Mat& frame, cv::Mat& mask);
  void ballHelperFunction(const cv::Mat& frame, const std::string& colorLabel);
  void eggHelperFunction(const cv::Mat& frame, const cv::Mat& mask);
  void frontAndBackHelperFunction(const cv::Mat& frame, cv::Mat& mask, std::string label);

  cv::Mat hsv_;
  std::shared_ptr<IBallProcessor> ballProcessor_;
  std::unique_ptr<WallProcessor> wallProcessor_;
};

#endif //IMAGEPROCESSOR_H

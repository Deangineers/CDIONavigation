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
  ImageProcessor();
  virtual ~ImageProcessor() = default;

  void processImage(const cv::Mat& frame);

protected:
  virtual void detectRedPixels(const cv::Mat& frame, const cv::Mat& overlay) = 0;
  virtual void detectBalls(const cv::Mat& frame, const cv::Mat& overlay) = 0;
  virtual void detectEgg(const cv::Mat& frame, const cv::Mat& overlay) = 0;
  virtual void detectFrontAndBack(const cv::Mat& frame, const cv::Mat& overlay) = 0;
  void redPixelHelperFunction(const cv::Mat& frame, cv::Mat& mask, const cv::Mat& overlay);
  void crossHelperFunction(const cv::Mat& frame, cv::Mat& mask, const cv::Mat& overlay);
  void ballHelperFunction(const cv::Mat& frame, const std::string& colorLabel, const cv::Mat& overlay);
  void eggHelperFunction(const cv::Mat& frame, const cv::Mat& mask, const cv::Mat& overlay);
  void frontAndBackHelperFunction(const cv::Mat& frame, cv::Mat& mask, std::string label, const cv::Mat& overlay);

  cv::Mat hsv_;
  std::shared_ptr<BallProcessor> ballProcessor_;
  std::unique_ptr<WallProcessor> wallProcessor_;
};

#endif //IMAGEPROCESSOR_H

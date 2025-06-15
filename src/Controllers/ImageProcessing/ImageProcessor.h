//
// Created by Elias Aggergaard Larsen on 15/06/2025.
//

#ifndef IIMAGEPROCESSOR_H
#define IIMAGEPROCESSOR_H
#include <opencv2/opencv.hpp>

class ImageProcessor
{
public:
  virtual ~ImageProcessor() = default;

  void processImage(const cv::Mat& frame);

protected:
  virtual void detectRedPixels(const cv::Mat& frame) = 0;
  virtual void detectBalls(const cv::Mat& frame) = 0;
  virtual void detectFrontAndBack(const cv::Mat& frame) = 0;
  void redPixelHelperFunction(const cv::Mat& frame, cv::Mat& mask);
  void ballHelperFunction(const cv::Mat& frame, cv::Mat& mask);
  void frontAndBackHelperFunction(const cv::Mat& frame, cv::Mat& mask, std::string label);

  cv::Mat hsv_;
};

#endif //IIMAGEPROCESSOR_H

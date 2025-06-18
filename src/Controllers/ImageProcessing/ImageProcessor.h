//
// Created by Elias Aggergaard Larsen on 15/06/2025.
//

#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H
#include <opencv2/opencv.hpp>

class ImageProcessor
{
public:
  virtual ~ImageProcessor() = default;

  void processImage(const cv::Mat& frame);

protected:
  virtual void detectRedPixels(const cv::Mat& frame) = 0;
  virtual void detectBalls(const cv::Mat& frame) = 0;
  virtual void detectEgg(const cv::Mat& frame) = 0;
  virtual void detectFrontAndBack(const cv::Mat& frame) = 0;
  void redPixelHelperFunction(const cv::Mat& frame, cv::Mat& mask);
  void crossHelperFunction(const cv::Mat& frame, cv::Mat& mask);
  void ballHelperFunction(const cv::Mat& frame, const cv::Mat& mask, const std::string& colorLabel);
  void eggHelperFunction(const cv::Mat& frame, const cv::Mat& mask);
  void frontAndBackHelperFunction(const cv::Mat& frame, cv::Mat& mask, std::string label);
  void findBallsInCorners(const cv::Mat& frame, const cv::Mat& mask);

  cv::Mat hsv_;
};

#endif //IMAGEPROCESSOR_H

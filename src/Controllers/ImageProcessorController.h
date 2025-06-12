//
// Created by Elias Aggergaard Larsen on 12/06/2025.
//

#ifndef IMAGEPROCESSORCONTROLLER_H
#define IMAGEPROCESSORCONTROLLER_H

#include <opencv2/opencv.hpp>
#include <string>

class ImageProcessorController
{
public:
  ImageProcessorController();

  cv::Mat applyHighContrast(const cv::Mat& frame);
  void detectRedPixels(const cv::Mat& frame);
  void detectBlackPixels(const cv::Mat& frame);
  void findAndCreate(cv::Mat& frame, const cv::Mat& hsv, const cv::Scalar& lower, const cv::Scalar& upper,
                     const std::string& label);
};

#endif //IMAGEPROCESSORCONTROLLER_H

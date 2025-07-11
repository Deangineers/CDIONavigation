//
// Created by Elias Aggergaard Larsen on 15/06/2025.
//

#ifndef CLOUDYIMAGEPROCESSOR_H
#define CLOUDYIMAGEPROCESSOR_H
#include "ImageProcessor.h"


class CloudyImageProcessor : public ImageProcessor
{
public:
  CloudyImageProcessor() = default;
  ~CloudyImageProcessor() override = default;

protected:
  void detectRedPixels(const cv::Mat& frame, const cv::Mat& overlay) override;
  void detectBalls(const cv::Mat& frame, const cv::Mat& overlay) override;
  void detectEgg(const cv::Mat& frame, const cv::Mat& overlay) override;
  void detectFrontAndBack(const cv::Mat& frame, const cv::Mat& overlay) override;
};


#endif //CLOUDYIMAGEPROCESSOR_H

//
// Created by Elias Aggergaard Larsen on 15/06/2025.
//

#ifndef CLOUDYIMAGEPROCESSOR_H
#define CLOUDYIMAGEPROCESSOR_H
#include "ImageProcessor.h"


class CloudyImageProcessor : public ImageProcessor
{
public:
  explicit CloudyImageProcessor(const std::shared_ptr<IBallProcessor>& ballProcessor);
  ~CloudyImageProcessor() override = default;

protected:
  void detectRedPixels(const cv::Mat& frame) override;
  void detectBalls(const cv::Mat& frame) override;
  void detectEgg(const cv::Mat& frame) override;
  void detectFrontAndBack(const cv::Mat& frame) override;
};


#endif //CLOUDYIMAGEPROCESSOR_H

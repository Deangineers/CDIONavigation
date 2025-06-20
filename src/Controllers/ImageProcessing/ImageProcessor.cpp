//
// Created by Elias Aggergaard Larsen on 15/06/2025.
//
#include "ImageProcessor.h"

#include "../MainController.h"
#include "../../Models/Vector.h"
#include "Utility/ConfigController.h"

ImageProcessor::ImageProcessor(const std::shared_ptr<IBallProcessor>& ballProcessor) :
  ballProcessor_(ballProcessor),
  wallProcessor_(std::make_unique<WallProcessor>())
{
}

void ImageProcessor::processImage(const cv::Mat& frame)
{
  cv::cvtColor(frame, hsv_, cv::COLOR_BGR2HSV);
  ballProcessor_->begin();
  detectRedPixels(frame);
  detectBalls(frame);
  detectEgg(frame);
  detectFrontAndBack(frame);
}

void ImageProcessor::redPixelHelperFunction(const cv::Mat& frame, cv::Mat& mask)
{
  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

  std::vector<cv::Point> nonZeroPoints;
  cv::findNonZero(mask, nonZeroPoints);

  if (nonZeroPoints.empty())
    return;

  cv::Point leftmost = nonZeroPoints[0];
  cv::Point rightmost = nonZeroPoints[0];
  cv::Point topmost = nonZeroPoints[0];
  cv::Point bottommost = nonZeroPoints[0];

  for (const auto& pt : nonZeroPoints)
  {
    if (pt.x < leftmost.x) leftmost = pt;
    if (pt.x > rightmost.x) rightmost = pt;
    if (pt.y < topmost.y) topmost = pt;
    if (pt.y > bottommost.y) bottommost = pt;
  }

  int shift = ConfigController::getConfigInt("CornerCrossHalfSize");
  leftmost.x += shift;
  rightmost.x -= shift;
  topmost.y += shift;
  bottommost.y -= shift;

  bool leftIsTop = std::abs(leftmost.y - topmost.y) < std::abs(leftmost.y - bottommost.y);

  cv::Point topLeft = leftIsTop ? topmost : leftmost;
  cv::Point bottomLeft = leftIsTop ? leftmost : bottommost;
  cv::Point topRight = leftIsTop ? rightmost : topmost;
  cv::Point bottomRight = leftIsTop ? bottommost : rightmost;

  int xLeftToRight = bottomRight.x - topLeft.x;
  int yLeftToRight = bottomRight.y - topLeft.y;

  int xRightToLeft = topRight.x - bottomLeft.x;
  int yRightToLeft = bottomLeft.y - topRight.y;

  Vector criss = {xLeftToRight, yLeftToRight};
  Vector cross = {xRightToLeft, yRightToLeft};

  if (criss.getLength() > cross.getLength())
  {
    topRight.x = bottomRight.x;
    topRight.y = topLeft.y;
    bottomLeft.x = topLeft.x;
    bottomLeft.y = bottomRight.y;
  }
  else
  {
    topLeft.x = bottomLeft.x;
    topLeft.y = topRight.y;
    bottomRight.x = topRight.x;
    bottomRight.y = bottomLeft.y;
  }

  std::vector<std::pair<cv::Point, cv::Point>> walls = {
    {topLeft, topRight},
    {topRight, bottomRight},
    {bottomRight, bottomLeft},
    {bottomLeft, topLeft}
  };

  for (const auto& [start, end] : walls)
  {
    Vector vec(end.x - start.x, end.y - start.y);
    auto wall = std::make_unique<VectorWithStartPos>(start.x, start.y, vec);

    MainController::addBlockedObject(std::move(wall));
    cv::line(frame, start, end, cv::Scalar(255, 0, 0), ConfigController::getConfigInt("WallWidth"), cv::LINE_AA);
  }
  cv::circle(frame, topLeft, 5, cv::Scalar(0, 255, 0), -1);
  cv::circle(frame, topRight, 5, cv::Scalar(0, 255, 0), -1);
  cv::circle(frame, bottomRight, 5, cv::Scalar(0, 255, 0), -1);
  cv::circle(frame, bottomLeft, 5, cv::Scalar(0, 255, 0), -1);
}


void ImageProcessor::crossHelperFunction(const cv::Mat& frame, cv::Mat& mask)
{
  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(mask, contours, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

  int label = 0;
  for (const auto& contour : contours)
  {
    double area = cv::contourArea(contour);
    if (area > ConfigController::getConfigInt("MaxCrossSize"))
    {
      continue;
    }
    // Approximate contour to get corners
    std::vector<cv::Point> approx;
    cv::approxPolyDP(contour, approx, 10, true); // epsilon=10 can be tuned

    // Draw vectors between points
    for (size_t i = 0; i < approx.size(); ++i)
    {
      cv::Point p1 = approx[i];
      cv::Point p2 = approx[(i + 1) % approx.size()]; // Loop back to start if needed
      Vector vector(p2.x - p1.x, p2.y - p1.y);
      if (vector.getLength() < ConfigController::getConfigInt("MinimumSizeOfBlockingObject"))
      {
        continue;
      }

      auto vectorToCreate = std::make_unique<VectorWithStartPos>(p1.x, p1.y, vector);
      if (not wallProcessor_->isWallValid(vectorToCreate.get()))
      {
        continue;
      }
      // Draw line (vector)
      cv::line(frame, p1, p2, cv::Scalar(255, 0, 0), ConfigController::getConfigInt("CrossWallWidth"),
               cv::LINE_AA, 0);
      cv::putText(frame, std::to_string(label++), p1, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
      MainController::addCrossObject(std::move(vectorToCreate));
      MainController::addCrossObject(std::make_unique<VectorWithStartPos>(p1.x, p1.y, vector));
    }
  }
}

void ImageProcessor::ballHelperFunction(const cv::Mat& frame, const cv::Mat& mask, const std::string& colorLabel)
{
  cv::Mat grey, hsv;
  cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

  cv::cvtColor(frame, grey, cv::COLOR_BGR2GRAY);
  std::vector<cv::Vec3f> circles;
  cv::HoughCircles(grey, circles, cv::HOUGH_GRADIENT, 1,
                   20, // minDist between centers
                   150, // param1: upper threshold for Canny
                   25, // param2: threshold for center detection
                   10, 40); // minRadius, maxRadius

  for (const auto& circle : circles)
  {
    int cx = cvRound(circle[0]);
    int cy = cvRound(circle[1]);
    int r = cvRound(circle[2]);

    cv::Vec3b hsvPixel = hsv.at<cv::Vec3b>(cy, cx);
    int h = hsvPixel[0];
    int s = hsvPixel[1];
    int v = hsvPixel[2];

    std::string detectedColor;
    if (s < 40 && v > 200)
    {
      detectedColor = "white";
    }
    else if (h >= 5 && h <= 50 && s > 100 && v > 100)
    {
      detectedColor = "orange";
    }
    else
    {
      detectedColor = "orange";
      //continue;
    }

    cv::Rect rect(cx - r, cy - r, 2 * r, 2 * r);
    if ((rect & cv::Rect(0, 0, frame.cols, frame.rows)) != rect)
      continue;

    double area = CV_PI * r * r;
    if (area < ConfigController::getConfigInt("MinimumSizeOfBlockingObject")
      || area > ConfigController::getConfigInt("EggBallDiffVal"))
    {
      continue;
    }
    std::string label = "ball";
    // at some point we might want to add the colorLabel here to separate white and orange balls

    int x1 = rect.x, y1 = rect.y;
    int x2 = x1 + rect.width, y2 = y1 + rect.height;

    auto courseObject = std::make_unique<CourseObject>(x1, y1, x2, y2, label);

    if (not ballProcessor_->isBallValid(courseObject.get()))
    {
      continue;
    }

    MainController::addCourseObject(std::move(courseObject));

    cv::rectangle(frame, rect, cv::Scalar(0, 255, 0), 2);
    cv::putText(frame, detectedColor, cv::Point(x1, y1 - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
    cv::circle(frame, cv::Point(cx, cy), 2, cv::Scalar(255, 0, 255), -1); // circle center
  }
}

void ImageProcessor::eggHelperFunction(const cv::Mat& frame, const cv::Mat& mask)
{
  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3)));

  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  for (const auto& cnt : contours)
  {
    double area = cv::contourArea(cnt);
    double perimeter = cv::arcLength(cnt, true);

    int eggMinSize = ConfigController::getConfigInt("EggBallDiffVal");
    int eggMaxSize = ConfigController::getConfigInt("EggMaxSize");

    int maxWidth = ConfigController::getConfigInt("maxEggWidth");
    int maxHeight = ConfigController::getConfigInt("maxEggHeight");

    if (area < eggMinSize || perimeter == 0 || area > eggMaxSize)
    {
      continue;
    }

    std::string label = "egg";

    double circularity = 4 * CV_PI * area / (perimeter * perimeter);
    if (circularity < 0.2)
    {
      continue;
    }

    cv::Rect rect = cv::boundingRect(cnt);
    int x1 = rect.x, y1 = rect.y;
    int x2 = x1 + rect.width, y2 = y1 + rect.height;
    if (rect.width > maxWidth || rect.height > maxHeight)
    {
      continue;
    }
    auto courseObject = std::make_unique<CourseObject>(x1, y1, x2, y2, label);

    if (not ballProcessor_->isEggValid(courseObject.get()))
    {
      continue;
    }

    MainController::addCourseObject(std::move(courseObject));
    cv::rectangle(frame, rect, cv::Scalar(0, 255, 0), 2);
    cv::putText(frame, label, cv::Point(x1, y1 - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);

    cv::Mat roi_mask = mask(rect);
    cv::GaussianBlur(roi_mask, roi_mask, cv::Size(5, 5), 0);
    cv::Mat edges;
    cv::Canny(roi_mask, edges, 50, 150);

    std::vector<cv::Point> edge_points;
    cv::findNonZero(edges, edge_points);
    for (const auto& pt : edge_points)
    {
      cv::circle(frame, pt + rect.tl(), 1, cv::Scalar(0, 0, 255), -1);
    }
  }
}


void ImageProcessor::frontAndBackHelperFunction(const cv::Mat& frame, cv::Mat& mask, std::string label)
{
  cv::Mat hsv;
  cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));

  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  for (const auto& cnt : contours)
  {
    double epsilon = 0.04 * cv::arcLength(cnt, true);
    std::vector<cv::Point> approx;
    cv::approxPolyDP(cnt, approx, epsilon, true);

    if (approx.size() == 4 && cv::isContourConvex(approx) && cv::contourArea(cnt) > ConfigController::getConfigInt(
      "MinAreaOfRobotFrontAndBack"))
    {
      cv::Rect rect = cv::boundingRect(approx);
      int x1 = rect.x, y1 = rect.y;
      int x2 = x1 + rect.width, y2 = y1 + rect.height;

      MainController::addCourseObject(std::make_unique<CourseObject>(x1, y1, x2, y2, label));
      cv::rectangle(frame, rect, cv::Scalar(0, 255, 0), 2);
      cv::putText(frame, label, cv::Point(x1, y1 - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0),
                  2);

      cv::Mat roi_mask = mask(rect);
      cv::GaussianBlur(roi_mask, roi_mask, cv::Size(5, 5), 0);
      cv::Mat edges;
      cv::Canny(roi_mask, edges, 50, 150);

      std::vector<cv::Point> edge_points;
      cv::findNonZero(edges, edge_points);
      for (const auto& pt : edge_points)
      {
        cv::circle(frame, pt + rect.tl(), 1, cv::Scalar(0, 0, 255), -1);
      }
    }
  }
}



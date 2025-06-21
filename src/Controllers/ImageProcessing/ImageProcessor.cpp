//
// Created by Elias Aggergaard Larsen on 15/06/2025.
//
#include "ImageProcessor.h"

#include <future>

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
  cv::Mat ballOverlay = cv::Mat::zeros(frame.size(), frame.type());
  cv::Mat eggOverlay = cv::Mat::zeros(frame.size(), frame.type());
  cv::Mat redOverlay = cv::Mat::zeros(frame.size(), frame.type());
  cv::Mat frontBackOverlay = cv::Mat::zeros(frame.size(), frame.type());


  auto f1 = std::async(std::launch::async, [&]
  {
    detectBalls(frame, ballOverlay);
  });

  auto f3 = std::async(std::launch::async, [&]
  {
    detectRedPixels(frame, redOverlay);
  });

  auto f4 = std::async(std::launch::async, [&]
  {
    detectFrontAndBack(frame, frontBackOverlay);
  });


  f1.get();
  f3.get();
  f4.get();
  auto applyOverlay = [](const cv::Mat& base, const cv::Mat& overlay)
  {
    cv::Mat gray;
    cv::cvtColor(overlay, gray, cv::COLOR_BGR2GRAY);
    overlay.copyTo(base, gray);
  };

  applyOverlay(frame, redOverlay);
  applyOverlay(frame, frontBackOverlay);
  applyOverlay(frame, ballOverlay);
  applyOverlay(frame, eggOverlay);
}

void ImageProcessor::redPixelHelperFunction(const cv::Mat& frame, cv::Mat& mask, const cv::Mat& overlay)
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

  if (not ConfigController::getConfigBool("UseFourPointsForWall"))
  {
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
    cv::line(overlay, start, end, cv::Scalar(255, 0, 0), ConfigController::getConfigInt("WallWidth"), cv::LINE_AA);
  }
  cv::circle(overlay, topLeft, 5, cv::Scalar(0, 255, 0), -1);
  cv::circle(overlay, topRight, 5, cv::Scalar(0, 255, 0), -1);
  cv::circle(overlay, bottomRight, 5, cv::Scalar(0, 255, 0), -1);
  cv::circle(overlay, bottomLeft, 5, cv::Scalar(0, 255, 0), -1);
}


void ImageProcessor::crossHelperFunction(const cv::Mat& frame, cv::Mat& mask, const cv::Mat& overlay)
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

    cv::Point top = cv::Point(INT_MIN, INT_MIN);
    cv::Point bottom = cv::Point(INT_MAX, INT_MAX);
    cv::Point right = cv::Point(INT_MIN, INT_MIN);
    cv::Point left = cv::Point(INT_MAX, INT_MAX);

    for (size_t i = 0; i < approx.size(); ++i)
    {
      cv::Point p = approx[i];
      if (p.y > top.y)
      {
        top = p;
      }

      if (p.y < bottom.y)
      {
        bottom = p;
      }

      if (p.x > right.x)
      {
        right = p;
      }

      if (p.x < left.x)
      {
        left = p;
      }
    }

    Vector horizontal = Vector(right.x - left.x, right.y - left.y);
    Vector vertical = Vector(top.x - bottom.x, top.y - bottom.y);

    MainController::addCrossObject(std::make_unique<VectorWithStartPos>(top.x, top.y, vertical));
    MainController::addCrossObject(std::make_unique<VectorWithStartPos>(right.x, right.y, horizontal));

    cv::line(overlay, bottom, top, cv::Scalar(255, 0, 0), ConfigController::getConfigInt("CrossWallWidth"),
               cv::LINE_AA, 0);
    cv::putText(overlay, std::to_string(label++), bottom, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);

    cv::line(overlay, left, right, cv::Scalar(255, 0, 0), ConfigController::getConfigInt("CrossWallWidth"),
               cv::LINE_AA, 0);
    cv::putText(overlay, std::to_string(label++), left, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
  }
}


void ImageProcessor::ballHelperFunction(const cv::Mat& frame, const std::string& colorLabel, const cv::Mat& overlay)
{
  cv::Mat grey;

  cv::cvtColor(frame, grey, cv::COLOR_BGR2GRAY);
  std::vector<cv::Vec3f> circles;
  // TODO, move to GPU
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

    cv::Vec3b hsvPixel = hsv_.at<cv::Vec3b>(cy, cx);
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
      //continue;
    }

    MainController::addCourseObject(std::move(courseObject));

    cv::rectangle(overlay, rect, cv::Scalar(0, 255, 0), 2);
    cv::putText(overlay, detectedColor, cv::Point(x1, y1 - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0),
                2);
    cv::circle(overlay, cv::Point(cx, cy), 2, cv::Scalar(255, 0, 255), -1); // circle center
  }
}

void ImageProcessor::eggHelperFunction(const cv::Mat& frame, const cv::Mat& mask, const cv::Mat& overlay)
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
    cv::rectangle(overlay, rect, cv::Scalar(0, 255, 0), 2);
    cv::putText(overlay, label, cv::Point(x1, y1 - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);

    cv::Mat roi_mask = mask(rect);
    cv::GaussianBlur(roi_mask, roi_mask, cv::Size(5, 5), 0);
    cv::Mat edges;
    cv::Canny(roi_mask, edges, 50, 150);

    std::vector<cv::Point> edge_points;
    cv::findNonZero(edges, edge_points);
    for (const auto& pt : edge_points)
    {
      cv::circle(overlay, pt + rect.tl(), 1, cv::Scalar(0, 0, 255), -1);
    }
  }
}

void ImageProcessor::frontAndBackHelperFunction(const cv::Mat &frame, cv::Mat &mask, std::string label,
                                                const cv::Mat &overlay) {
  // Preprocessing
  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));

  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  for (const auto& cnt : contours)
  {
    double epsilon = 0.04 * cv::arcLength(cnt, true);
    std::vector<cv::Point> approx;
    cv::approxPolyDP(cnt, approx, epsilon, true);

    // Rectangle filter
    if (approx.size() == 4 && cv::isContourConvex(approx))
    {
      double area = cv::contourArea(cnt);
      if (area < ConfigController::getConfigInt("MinAreaOfRobotFrontAndBack"))
        continue;

      cv::Rect rect = cv::boundingRect(approx);
      int x1 = rect.x, y1 = rect.y;
      int x2 = x1 + rect.width, y2 = y1 + rect.height;

      auto courseObject = std::make_unique<CourseObject>(x1, y1, x2, y2, label);
      MainController::addCourseObject(std::move(courseObject));

      // Draw debug visuals
      cv::rectangle(overlay, rect, cv::Scalar(0, 255, 0), 2);
      cv::putText(overlay, label, cv::Point(x1, y1 - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);

      // Optional: visualize edge points
      cv::Mat roi_mask = mask(rect);
      cv::GaussianBlur(roi_mask, roi_mask, cv::Size(5, 5), 0);
      cv::Mat edges;
      cv::Canny(roi_mask, edges, 50, 150);

      std::vector<cv::Point> edge_points;
      cv::findNonZero(edges, edge_points);
      for (const auto& pt : edge_points)
      {
        cv::circle(overlay, pt + rect.tl(), 1, cv::Scalar(0, 0, 255), -1);
      }
    }
  }
}




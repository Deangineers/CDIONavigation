//
// Created by Elias Aggergaard Larsen on 15/06/2025.
//
#include "ImageProcessor.h"

#include <future>

#include "../MainController.h"
#include "../ObjectCounter.h"
#include "../../Models/Vector.h"
#include "Utility/ConfigController.h"


ImageProcessor::ImageProcessor() : ballProcessor_(std::make_shared<BallProcessor>()),wallProcessor_(std::make_unique<WallProcessor>())
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


  auto applyOverlay = [](const cv::Mat& base, const cv::Mat& overlay)
  {
      cv::Mat gray;
      cv::cvtColor(overlay, gray, cv::COLOR_BGR2GRAY);
      overlay.copyTo(base, gray);
  };

  f1.get();
  applyOverlay(frame, ballOverlay);
  f3.get();
  applyOverlay(frame, redOverlay);
  f4.get();
  applyOverlay(frame, frontBackOverlay);
}

void ImageProcessor::redPixelHelperFunction(const cv::Mat& frame, cv::Mat& mask, const cv::Mat& overlay)
{
  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
  cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

  std::vector<cv::Point> nonZeroPoints;
  cv::findNonZero(mask, nonZeroPoints);

  if (nonZeroPoints.empty())
    return;

  int midX = mask.cols / 2;
  int midY = mask.rows / 2;

  cv::Point topLeftMinX(mask.cols, 0), topLeftMinY(0, mask.rows);
  cv::Point topRightMaxX(0, 0), topRightMinY(mask.cols, mask.rows);
  cv::Point bottomLeftMinX(mask.cols, 0), bottomLeftMaxY(0, 0);
  cv::Point bottomRightMaxX(0, 0), bottomRightMaxY(0, 0);

  for (const auto& pt : nonZeroPoints)
  {
    if (pt.x <= midX && pt.y <= midY)
    {
      if (pt.x < topLeftMinX.x) topLeftMinX = pt;
      if (pt.y < topLeftMinY.y) topLeftMinY = pt;
    }
    else if (pt.x > midX && pt.y <= midY)
    {
      if (pt.x > topRightMaxX.x) topRightMaxX = pt;
      if (pt.y < topRightMinY.y) topRightMinY = pt;
    }
    else if (pt.x <= midX && pt.y > midY)
    {
      if (pt.x < bottomLeftMinX.x) bottomLeftMinX = pt;
      if (pt.y > bottomLeftMaxY.y) bottomLeftMaxY = pt;
    }
    else if (pt.x > midX && pt.y > midY)
    {
      if (pt.x > bottomRightMaxX.x) bottomRightMaxX = pt;
      if (pt.y > bottomRightMaxY.y) bottomRightMaxY = pt;
    }
  }

  int shift = ConfigController::getConfigInt("CornerCrossHalfSize");

  auto topLeft = cv::Point((topLeftMinX.x + topLeftMinY.x + shift)/2, (topLeftMinY.y + topLeftMinX.y + shift)/2);
  auto bottomLeft = cv::Point((bottomLeftMinX.x + bottomLeftMaxY.x + shift)/2, (bottomLeftMaxY.y + bottomLeftMinX.y - shift)/2);
  auto topRight = cv::Point((topRightMaxX.x + topRightMinY.x - shift)/2, (topRightMinY.y + topRightMaxX.y + shift)/2);
  auto bottomRight = cv::Point((bottomRightMaxX.x + bottomRightMaxY.x - shift)/2, (bottomRightMaxY.y + bottomRightMaxX.y - shift)/2);

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
    if (area > ConfigController::getConfigInt("MaxCrossSize") || area < 700)
      continue;

    std::vector<cv::Point> approx;
    cv::approxPolyDP(contour, approx, 10, true); // epsilon=10 can be tuned

    if (approx.size() < 4)
      continue;

    // Step 1: Find the longest line (first axis)
    double maxDist = 0.0;
    cv::Point p1_best, p2_best;
    for (size_t i = 0; i < approx.size(); ++i)
    {
      for (size_t j = i + 1; j < approx.size(); ++j)
      {
        double dist = cv::norm(approx[i] - approx[j]);
        if (dist > maxDist)
        {
          maxDist = dist;
          p1_best = approx[i];
          p2_best = approx[j];
        }
      }
    }

    // Step 2: Find a roughly orthogonal line (second axis)
    cv::Point2f axis1 = p2_best - p1_best;
    cv::Point2f axis1_unit = axis1 * (1.0 / cv::norm(axis1));
    cv::Point p3_best, p4_best;
    double bestScore = 0.0;

    for (size_t i = 0; i < approx.size(); ++i)
    {
      for (size_t j = i + 1; j < approx.size(); ++j)
      {
        if ((approx[i] == p1_best && approx[j] == p2_best) || (approx[j] == p1_best && approx[i] == p2_best))
          continue;

        cv::Point2f vec = approx[j] - approx[i];
        double vecLength = cv::norm(vec);
        if (vecLength < 10) continue;

        cv::Point2f unitVec = vec * (1.0 / vecLength);
        double dot = std::abs(axis1_unit.dot(unitVec)); // want close to 0

        if (dot < 0.3 && vecLength > bestScore)
        {
          bestScore = vecLength;
          p3_best = approx[i];
          p4_best = approx[j];
        }
      }
    }

    // If orthogonal axis not found, skip
    if (bestScore == 0.0)
      continue;

    // Compute midpoints (used as origin for each arm vector)
    cv::Point topCenter = (p1_best + p2_best) * 0.5;
    cv::Point sideCenter = (p3_best + p4_best) * 0.5;

    // Create vectors
    Vector vertical(p2_best.x - p1_best.x, p2_best.y - p1_best.y);
    Vector horizontal(p4_best.x - p3_best.x, p4_best.y - p3_best.y);

    MainController::addCrossObject(std::make_unique<VectorWithStartPos>(topCenter.x, topCenter.y, vertical));
    MainController::addCrossObject(std::make_unique<VectorWithStartPos>(sideCenter.x, sideCenter.y, horizontal));

    // Draw vectors for visualization
    cv::line(overlay, p1_best, p2_best, cv::Scalar(255, 0, 0), ConfigController::getConfigInt("CrossWallWidth"), cv::LINE_AA);
    cv::putText(overlay, std::to_string(label++), p1_best, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);

    cv::line(overlay, p3_best, p4_best, cv::Scalar(255, 0, 0), ConfigController::getConfigInt("CrossWallWidth"), cv::LINE_AA);
    cv::putText(overlay, std::to_string(label++), p3_best, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
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
    else if (h >= 30 && h <= 85 && s > 35 && v > 35)
    {
      detectedColor = "robotFront";
    }
    else if (h >= 120 && h <= 170 && s > 85 && v > 85)
    {
      detectedColor = "robotBack";
    }

    if (detectedColor.empty())
    {
      continue;
    }

    cv::Rect rect(cx - r, cy - r, 2 * r, 2 * r);
    if ((rect & cv::Rect(0, 0, frame.cols, frame.rows)) != rect)
      continue;

    double area = CV_PI * r * r;
    if (area < ConfigController::getConfigInt("MinimumSizeOfBlockingObject")
      || area > ConfigController::getConfigInt("EggBallDiffVal") && detectedColor != "robotFront" && detectedColor != "robotBack")
    {
      continue;
    }
    if ((detectedColor == "robotFront" || detectedColor == "robotBack") && area < 1850)
    {
      detectedColor = "orange";
    }
    std::string label = "ball";
    // at some point we might want to add the colorLabel here to separate white and orange balls

    int x1 = rect.x, y1 = rect.y;
    int x2 = x1 + rect.width, y2 = y1 + rect.height;

    auto courseObject = std::make_unique<CourseObject>(x1, y1, x2, y2, detectedColor);
    ObjectCounter::objectDetected(detectedColor);

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
      ObjectCounter::objectDetected(label);
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



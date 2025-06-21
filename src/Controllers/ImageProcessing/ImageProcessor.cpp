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

  auto f2 = std::async(std::launch::async, [&]
  {
    detectEgg(frame, eggOverlay);
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
  f2.get();
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

  int shift = ConfigController::getConfigInt("CornerCrossHalfSize");

  cv::Point topLeft, topRight, bottomLeft, bottomRight;

  if (!ConfigController::getConfigBool("UseFourPointsForWall"))
  {
    cv::Rect bounding = cv::boundingRect(nonZeroPoints);

    bounding.x += shift;
    bounding.y += shift;
    bounding.width = std::max(0, bounding.width - 2 * shift);
    bounding.height = std::max(0, bounding.height - 2 * shift);

    topLeft = cv::Point(bounding.x, bounding.y);
    topRight = cv::Point(bounding.x + bounding.width, bounding.y);
    bottomLeft = cv::Point(bounding.x, bounding.y + bounding.height);
    bottomRight = cv::Point(bounding.x + bounding.width, bounding.y + bounding.height);
  }
  else
  {
    // Shrink mask slightly to pull polygon inside actual red region
    cv::Mat eroded;
    cv::Mat shrinkKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::erode(mask, eroded, shrinkKernel);

// Find contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(eroded, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);


    if (contours.empty()) return;

    auto largestContour = *std::max_element(contours.begin(), contours.end(),
                                            [](const auto& a, const auto& b) {
                                                return cv::contourArea(a) < cv::contourArea(b);
                                            });

    std::vector<cv::Point> approx;
    double epsilon = 0.02 * cv::arcLength(largestContour, true);
    cv::approxPolyDP(largestContour, approx, epsilon, true);

    if (approx.size() != 4) {
      std::cerr << "Warning: approxPolyDP did not return 4 points (got " << approx.size() << ")\n";
      return;
    }

    // Sort the corners consistently: TL, TR, BR, BL
    auto sortCorners = [](const std::vector<cv::Point>& pts) -> std::vector<cv::Point> {
        std::vector<cv::Point> sorted = pts;

        // Compute the centroid
        cv::Point2f center(0.f, 0.f);
        for (const auto& pt : sorted)
          center += cv::Point2f(static_cast<float>(pt.x), static_cast<float>(pt.y));
        center *= (1.0f / sorted.size());

        std::vector<cv::Point> top, bottom;
        for (const auto& pt : sorted) {
          if (pt.y < center.y)
            top.push_back(pt);
          else
            bottom.push_back(pt);
        }

        if (top.size() != 2 || bottom.size() != 2) {
          std::cerr << "Corner sorting failed due to incorrect top/bottom split\n";
          return pts; // fallback to original
        }

        cv::Point topLeft = top[0].x < top[1].x ? top[0] : top[1];
        cv::Point topRight = top[0].x > top[1].x ? top[0] : top[1];
        cv::Point bottomLeft = bottom[0].x < bottom[1].x ? bottom[0] : bottom[1];
        cv::Point bottomRight = bottom[0].x > bottom[1].x ? bottom[0] : bottom[1];

        return {topLeft, topRight, bottomRight, bottomLeft};
    };

    auto ordered = sortCorners(approx);
    topLeft = ordered[0];
    topRight = ordered[1];
    bottomRight = ordered[2];
    bottomLeft = ordered[3];
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

    cv::Point top(INT_MIN, INT_MIN), bottom(INT_MAX, INT_MAX);
    cv::Point left(INT_MAX, INT_MAX), right(INT_MIN, INT_MIN);

    for (const auto& p : contour)
    {
      if (p.y > top.y) top = p;
      if (p.y < bottom.y) bottom = p;
      if (p.x > right.x) right = p;
      if (p.x < left.x) left = p;
    }

    cv::Moments M = cv::moments(contour);
    if (M.m00 == 0) continue;
    cv::Point center(M.m10 / M.m00, M.m01 / M.m00);

    cv::Point topArm(center.x, center.y - (center.y - bottom.y));
    cv::Point bottomArm(center.x, center.y + (top.y - center.y));
    Vector verticalVec(0, bottomArm.y - topArm.y);

    cv::Point leftArm(center.x - (right.x - center.x), center.y);
    cv::Point rightArm(center.x + (center.x - left.x), center.y);
    Vector horizontalVec(rightArm.x - leftArm.x, 0);

    MainController::addCrossObject(std::make_unique<VectorWithStartPos>(topArm.x, topArm.y, verticalVec));
    MainController::addCrossObject(std::make_unique<VectorWithStartPos>(leftArm.x, leftArm.y, horizontalVec));

    cv::line(overlay, topArm, bottomArm, cv::Scalar(255, 0, 0),
             ConfigController::getConfigInt("CrossWallWidth"), cv::LINE_AA);
    cv::putText(overlay, std::to_string(label++), topArm, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);

    cv::line(overlay, leftArm, rightArm, cv::Scalar(255, 0, 0),
             ConfigController::getConfigInt("CrossWallWidth"), cv::LINE_AA);
    cv::putText(overlay, std::to_string(label++), leftArm, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
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

void ImageProcessor::frontAndBackHelperFunction(const cv::Mat &frame, cv::Mat &maskInput, std::string label,
                                                const cv::Mat &overlay) {

  cv::Mat mask;
  maskInput.copyTo(mask);

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




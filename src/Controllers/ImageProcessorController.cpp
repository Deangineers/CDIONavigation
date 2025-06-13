//
// Created by Elias Aggergaard Larsen on 12/06/2025.
//

#include "ImageProcessorController.h"

#include "MainController.h"
#include "Utility/ConfigController.h"
#include "../Models/BlockingObject.h"
#include "../Models/Vector.h"

ImageProcessorController::ImageProcessorController()
{
}

cv::Mat ImageProcessorController::applyHighContrast(const cv::Mat& frame)
{
    cv::Mat lab;
    cv::cvtColor(frame, lab, cv::COLOR_BGR2Lab);

    std::vector<cv::Mat> lab_planes(3);
    cv::split(lab, lab_planes);

    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(3.0, cv::Size(8, 8));
    clahe->apply(lab_planes[0], lab_planes[0]);

    cv::merge(lab_planes, lab);
    cv::Mat enhanced;
    cv::cvtColor(lab, enhanced, cv::COLOR_Lab2BGR);

    cv::Mat kernel = (cv::Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
    cv::Mat sharpened;
    cv::filter2D(enhanced, sharpened, -1, kernel);

    return sharpened;
}

void ImageProcessorController::detectRedPixels(const cv::Mat& frame)
{
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    cv::Mat mask1, mask2, redMask;
    cv::inRange(hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), mask1);
    cv::inRange(hsv, cv::Scalar(160, 100, 100), cv::Scalar(180, 255, 255), mask2);
    cv::bitwise_or(mask1, mask2, redMask);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(redMask, redMask, cv::MORPH_OPEN, kernel);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(redMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours)
    {
        double area = cv::contourArea(contour);
        if (area < ConfigController::getConfigInt("minRedSize"))
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

            // Draw line (vector)
            cv::arrowedLine(frame, p1, p2, cv::Scalar(255, 0, 0), ConfigController::getConfigInt("WallWidth"),
                            cv::LINE_AA, 0, 0.01); // green arrows
            Vector vector(p2.x - p1.x, p2.y - p1.y);
            MainController::addBlockedObject(std::make_unique<BlockingObject>(p1.x, p1.y, vector));
        }
    }
}

void ImageProcessorController::detectBlackPixels(const cv::Mat& frame)
{
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    cv::Mat mask;
    cv::inRange(hsv, cv::Scalar(0, 0, 0), cv::Scalar(180, 255, 50), mask);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);

    std::vector<cv::Point> blackPoints;
    cv::findNonZero(mask, blackPoints);
}

void ImageProcessorController::detectBalls(const cv::Mat& frame)
{
    cv::Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

    // Define color ranges for orange and white
    cv::Mat orangeMask, whiteMask, mask;
    cv::inRange(hsv, cv::Scalar(5, 50, 50), cv::Scalar(30, 255, 255), orangeMask); // Orange
    cv::inRange(hsv, cv::Scalar(0, 0, 200), cv::Scalar(180, 40, 255), whiteMask); // White

    cv::bitwise_or(orangeMask, whiteMask, mask);
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& cnt : contours)
    {
        double area = cv::contourArea(cnt);
        double perimeter = cv::arcLength(cnt, true);

        int minimumBallSize = ConfigController::getConfigInt("MinimumBallSize");
        int eggBallDiffVal = ConfigController::getConfigInt("EggBallDiffVal");

        if (area < minimumBallSize || perimeter == 0)
            continue;

        std::string label = area > eggBallDiffVal ? "egg" : "ball";

        // Optional: Circularity check
        double circularity = 4 * CV_PI * area / (perimeter * perimeter);
        if (circularity < 0.6) // adjust threshold as needed
            continue;

        cv::Rect rect = cv::boundingRect(cnt);
        int x1 = rect.x, y1 = rect.y;
        int x2 = x1 + rect.width, y2 = y1 + rect.height;

        MainController::addCourseObject(std::make_unique<CourseObject>(x1, y1, x2, y2, label));

        // Draw bounding box and label
        cv::rectangle(frame, rect, cv::Scalar(0, 255, 0), 2);
        cv::putText(frame, label, cv::Point(x1, y1 - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);

        // Optional: visualize edge points
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


void ImageProcessorController::findAndCreate(cv::Mat& frame, const cv::Mat& hsv,
                                             const cv::Scalar& lower, const cv::Scalar& upper,
                                             const std::string& label)
{
    cv::Mat mask;
    cv::inRange(hsv, lower, upper, mask);
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto& cnt : contours)
    {
        double epsilon = 0.04 * cv::arcLength(cnt, true);
        std::vector<cv::Point> approx;
        cv::approxPolyDP(cnt, approx, epsilon, true);

        if (approx.size() == 4 && cv::isContourConvex(approx) && cv::contourArea(cnt) > 100)
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

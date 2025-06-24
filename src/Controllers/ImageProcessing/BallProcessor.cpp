//
// Created by Elias Aggergaard Larsen on 18/06-2025.
//

#include "BallProcessor.h"

#include "../../../Simulator/TextureLocations.h"
#include "Utility/ConfigController.h"

BallProcessor::BallProcessor() : ballsFromImages_(ConfigController::getConfigInt("ImagesToAverage")), eggFromImage_(ConfigController::getConfigInt("ImagesToAverage"))
{
}

void BallProcessor::begin()
{
  currentIndex_++;
  currentIndex_ %= ConfigController::getConfigInt("ImagesToAverage");
  ballsFromImages_[currentIndex_].clear();
  eggFromImage_[currentIndex_].clear();
}

bool BallProcessor::isBallValid(CourseObject* courseObject)
{
  const bool toAdd = toAddBall(courseObject);
  ballsFromImages_[currentIndex_].emplace_back(*courseObject);
  return toAdd;
}

bool BallProcessor::isEggValid(const CourseObject* courseObject)
{
  eggFromImage_[currentIndex_].emplace_back(*courseObject);
  return toAddEgg(courseObject);
}

bool BallProcessor::toAddBall(const CourseObject* courseObject) const
{
  for (const auto& egg : eggFromImage_[currentIndex_])
  {
    int centerX = (courseObject->x1() + courseObject->x2()) / 2;
    int centerY = (courseObject->y1() + courseObject->y2()) / 2;

    if ((std::abs(egg.x1() - courseObject->x1()) < 20 || std::abs(egg.x2() - courseObject->x2()) < 20) &&
      std::abs(egg.y1() - courseObject->y2()) < 20 || std::abs(egg.y2() - courseObject->y2()) < 20)
    {
      return false;
    }
  }
  return true;
}

bool BallProcessor::toAddEgg(const CourseObject* courseObject) const
{
  int timesSeen = 0;
  for (const auto& outerVector : ballsFromImages_)
  {
    for (const auto& ball : outerVector)
    {
      if (courseObject->courseObjectWithinValidRange(&ball))
      {
        timesSeen++;
      }
    }
  }

  return timesSeen < 2;
}


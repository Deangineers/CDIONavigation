//
// Created by Elias Aggergaard Larsen on 18/06-2025.
//

#include "BallProcessor.h"

#include "Utility/ConfigController.h"

BallProcessor::BallProcessor() : ballsFromImages_(ConfigController::getConfigInt("ImagesToAverage"))
{
}

void BallProcessor::begin()
{
  currentIndex_++;
  currentIndex_ %= ConfigController::getConfigInt("ImagesToAverage");
  ballsFromImages_[currentIndex_].clear();
}

bool BallProcessor::isBallValid(CourseObject* courseObject)
{
  const bool toAdd = toAddBall(courseObject);
  ballsFromImages_[currentIndex_].emplace_back(*courseObject);
  return toAdd;
}

bool BallProcessor::toAddBall(const CourseObject* courseObject) const
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
  return timesSeen >= ConfigController::getConfigInt("AmountOfSeenBeforeCreate");
}


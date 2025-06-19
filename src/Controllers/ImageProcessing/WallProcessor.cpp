//
// Created by fbas on 6/19/25.
//

#include "WallProcessor.h"

#include "Utility/ConfigController.h"

WallProcessor::WallProcessor() : wallsFromImages_(ConfigController::getConfigInt("ImagesToAverage"))
{
}

void WallProcessor::begin()
{
  currentIndex_++;
  currentIndex_ %= ConfigController::getConfigInt("ImagesToAverage");
  wallsFromImages_[currentIndex_].clear();
}

bool WallProcessor::isWallValid(VectorWithStartPos* vector)
{
  const bool toAdd = toAddWall(vector);
  wallsFromImages_[currentIndex_].emplace_back(*vector);
  return toAdd;
}

bool WallProcessor::toAddWall(VectorWithStartPos* vector) const
{
  int timesSeen = 0;
  for (const auto& outerVector : wallsFromImages_)
  {
    for (const auto& wall : outerVector)
    {
      if (wall.isSameVector(*vector))
      {
        timesSeen++;
      }
    }
  }
  return timesSeen >= ConfigController::getConfigInt("WallSeenBeforeCreate");
}

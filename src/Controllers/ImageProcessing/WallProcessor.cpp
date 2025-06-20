//
// Created by fbas on 6/19/25.
//

#include "WallProcessor.h"

#include "Utility/ConfigController.h"

WallProcessor::WallProcessor() : wallsFromImages_(ConfigController::getConfigInt("ImagesToAverageWall")), wallsLocked_(false)
{
}

void WallProcessor::begin()

{
  if (wallsLocked_) return;

  currentIndex_++;
  currentIndex_ %= ConfigController::getConfigInt("ImagesToAverageWall");
  wallsFromImages_[currentIndex_].clear();
}

bool WallProcessor::isWallValid(VectorWithStartPos* vector)
{
  if (wallsLocked_)
  {

    for (const auto& wall : finalWalls_)
    {
      if (wall.isSameVector(*vector))
      {
        return true;
      }
    }
    return false;
  }

  const bool toAdd = toAddWall(vector);
  wallsFromImages_[currentIndex_].emplace_back(*vector);

  if (currentIndex_ == ConfigController::getConfigInt("ImagesToAverage") - 1)
  {
    finalizeWalls();
  }

  return toAdd;
}

void WallProcessor::finalizeWalls()
{
  struct VectorCount
  {
      VectorWithStartPos vector;
      int count;
  };

  std::vector<VectorCount> vectorCounts;

  for (const auto& frameVectors : wallsFromImages_)
  {
    for (const auto& vec : frameVectors)
    {

      bool found = false;
      for (auto& vc : vectorCounts)
      {
        if (vc.vector.isSameVector(vec))
        {
          vc.count++;
          found = true;
          break;
        }
      }
      if (!found)
      {
        vectorCounts.push_back({vec, 1});
      }
    }
  }

  finalWalls_.clear();
  int threshold = ConfigController::getConfigInt("WallSeenBeforeCreate");
  for (const auto& vc : vectorCounts)
  {
    if (vc.count >= threshold)
    {
      finalWalls_.emplace_back(vc.vector);
    }
  }

  wallsLocked_ = true;
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

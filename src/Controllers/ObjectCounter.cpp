//
// Created by localadmin on 21/06/2025.
//

#include "ObjectCounter.h"

void ObjectCounter::objectDetected(std::string label)
{
  map_[label]++;
}

void ObjectCounter::reset()
{
  map_.clear();
}

int ObjectCounter::getCount(std::string label)
{
  return map_[label];
}

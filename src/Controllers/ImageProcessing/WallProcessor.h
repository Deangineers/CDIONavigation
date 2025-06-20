//
// Created by fbas on 6/19/25.
//

#ifndef WALLPROCESSOR_H
#define WALLPROCESSOR_H
#include <vector>
#include <string>
#include <unordered_map>
#include "../../Models/VectorWithStartPos.h"

class WallProcessor
{
public:
  WallProcessor();
  void begin();
  bool isWallValid(VectorWithStartPos* vector);
private:
  bool toAddWall(VectorWithStartPos* vector) const;
  void finalizeWalls();
  int currentIndex_ = 0;
  bool wallsLocked_;
  std::vector<std::vector<VectorWithStartPos>> wallsFromImages_;
  std::vector<VectorWithStartPos> finalWalls_;
};



#endif //WALLPROCESSOR_H

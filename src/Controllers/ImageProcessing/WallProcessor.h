//
// Created by fbas on 6/19/25.
//

#ifndef WALLPROCESSOR_H
#define WALLPROCESSOR_H
#include <vector>
#include "../../Models/VectorWithStartPos.h"

class WallProcessor
{
public:
  WallProcessor();
  void begin();
  bool isWallValid(VectorWithStartPos* vector);
private:
  bool toAddWall(VectorWithStartPos* vector) const;
  int currentIndex_ = 0;
  std::vector<std::vector<VectorWithStartPos>> wallsFromImages_;
};



#endif //WALLPROCESSOR_H

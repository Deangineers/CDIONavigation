//
// Created by Elias Aggergaard Larsen on 15/06/2025.
//

#ifndef EGG_H
#define EGG_H
#include <vector>

#include "VectorWithStartPos.h"


class Egg final : public VectorWithStartPos
{
public:
  Egg(int x1, int y1, int x2, int y2);
  [[nodiscard]] Vector closestVectorFromPoint(const Vector& fromPoint) const override;

private:
  std::vector<VectorWithStartPos> vectors_;;
};


#endif //EGG_H

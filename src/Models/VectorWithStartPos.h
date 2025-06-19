//
// Created by fbas on 13/06/25.
//

#ifndef VECTORWITHSTARTPOS_H
#define VECTORWITHSTARTPOS_H

#include "Vector.h"


class VectorWithStartPos : public Vector
{
public:
  VectorWithStartPos(int x, int y, const Vector& vector);
  bool isSameVector(const VectorWithStartPos &other) const;
  [[nodiscard]] int startX() const;
  [[nodiscard]] int startY() const;
  [[nodiscard]] Vector minimalPoints() const;
  [[nodiscard]] Vector maxPoints() const;
  [[nodiscard]] virtual Vector closestVectorFromPoint(const Vector& fromPoint) const;
  int startX_;
  int startY_;

  [[nodiscard]] int getLowestX() const;
  [[nodiscard]] int getLowestY() const;
  [[nodiscard]] int getMaxX() const;
  [[nodiscard]] int getMaxY() const;
protected:
};


#endif //VECTORWITHSTARTPOS_H

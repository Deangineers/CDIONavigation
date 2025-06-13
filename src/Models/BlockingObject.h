//
// Created by fbas on 13/06/25.
//

#ifndef BLOCKINGOBJECT_H
#define BLOCKINGOBJECT_H

#include "Vector.h"


class BlockingObject {
public:
  BlockingObject(int x, int y, Vector vector);
  [[nodiscard]] int x() const;
  [[nodiscard]] int y() const;
  [[nodiscard]] Vector vector() const;
  Vector minimalPoints() const;
  Vector maxPoints() const;

private:
  int getLowestX() const;
  int getLowestY() const;
  int getMaxX() const;
  int getMaxY() const;
  int x_;
  int y_;
  Vector vector_;
};



#endif //BLOCKINGOBJECT_H

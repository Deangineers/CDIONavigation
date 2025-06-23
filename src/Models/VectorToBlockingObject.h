//
// Created by localadmin on 23/06/2025.
//

#ifndef VECTORTOBLOCKINGOBJECT_H
#define VECTORTOBLOCKINGOBJECT_H
#include "Vector.h"


class VectorToBlockingObject
{
public:
  VectorToBlockingObject(const Vector& vector, bool isCross);

  Vector vector;
  bool isCross;
};



#endif //VECTORTOBLOCKINGOBJECT_H

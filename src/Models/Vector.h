//
// Created by Elias Aggergaard Larsen on 11/06/2025.
//

#ifndef VECTOR_H
#define VECTOR_H
#include <string>


class Vector
{
public:
  Vector(int x, int y);
  bool isNullVector();
  double getLength();
  std::string toString();
  int x, y;
};


#endif //VECTOR_H

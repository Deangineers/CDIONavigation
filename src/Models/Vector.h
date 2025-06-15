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
  virtual ~Vector() = default;

  Vector operator-(const Vector& other) const;

  Vector operator+(const Vector& other) const;

  Vector operator*(double scalar) const;

  double dot(const Vector& a, const Vector& b) const;

  Vector closestVectorFromPoint(const Vector& vectorStart, const Vector& fromPoint) const;

  [[nodiscard]] bool isNullVector() const;
  [[nodiscard]] double getLength() const;
  [[nodiscard]] std::string toString() const;
  [[nodiscard]] bool hasSmallerValueThan(Vector v) const;
  [[nodiscard]] int getSmallestValue() const;
  int x, y;
};


#endif //VECTOR_H

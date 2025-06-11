//
// Created by Elias Aggergaard Larsen on 01/05/2025.
//

#ifndef COURSEOBJECT_H
#define COURSEOBJECT_H
#include <string>


class CourseObject
{
public:
  CourseObject(int x1, int y1, int x2, int y2, std::string name);
  [[nodiscard]] int x1() const;
  [[nodiscard]] int y1() const;
  [[nodiscard]] int x2() const;
  [[nodiscard]] int y2() const;
  [[nodiscard]] std::string name() const;

  void shiftX(int x);
  void shiftY(int y);

private:
  int x1_, y1_, x2_, y2_;
  std::string name_;
};


#endif //COURSEOBJECT_H

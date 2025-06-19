//
// Created by Elias Aggergaard Larsen on 19/06-2025.
//

#ifndef IBALLPROCESSOR_H
#define IBALLPROCESSOR_H
#include "../../Models/CourseObject.h"

class IBallProcessor
{
public:
  virtual ~IBallProcessor() = default;
  virtual void begin() = 0;
  virtual bool isBallValid(CourseObject* courseObject) = 0;
  virtual bool isEggValid(const CourseObject* courseObject) = 0;
};

#endif //IBALLPROCESSOR_H

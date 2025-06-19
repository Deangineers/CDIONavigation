//
// Created by Elias Aggergaard Larsen on 18/06-2025.
//

#ifndef BALLPROCESSOR_H
#define BALLPROCESSOR_H
#include <opencv2/core/mat.hpp>

#include "IBallProcessor.h"
#include "../../Models/CourseObject.h"


class BallProcessor : public IBallProcessor
{
public:
  BallProcessor();
  void begin() override;
  bool isBallValid(CourseObject* courseObject) override;
  bool isEggValid(const CourseObject* courseObject) override;

private:
  bool toAddBall(const CourseObject* courseObject) const;
  bool toAddEgg(const CourseObject* courseObject) const;
  int currentIndex_ = 0;
  std::vector<std::vector<CourseObject>> ballsFromImages_;
};


#endif //BALLPROCESSOR_H

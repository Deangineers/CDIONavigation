//
// Created by Elias Aggergaard Larsen on 20/06-2025.
//

#ifndef TESTBALLPROCESSOR_H
#define TESTBALLPROCESSOR_H
#include "../src/Controllers/ImageProcessing/BallProcessor.h"
#include "../src/Controllers/ImageProcessing/IBallProcessor.h"


class TestBallProcessor : public IBallProcessor
{
public:
  TestBallProcessor();
  ~TestBallProcessor() override = default;
  void begin() override;
  bool isBallValid(CourseObject* courseObject) override;
  bool isEggValid(const CourseObject* courseObject) override;
  int getBallCounter();
  int getEggCounter();

private:
  std::unique_ptr<BallProcessor> ballProcessor_;
  int ballCounter_;
  int eggCounter_;
};


#endif //TESTBALLPROCESSOR_H

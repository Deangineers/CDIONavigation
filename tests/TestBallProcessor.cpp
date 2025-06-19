//
// Created by Elias Aggergaard Larsen on 20/06-2025.
//

#include "TestBallProcessor.h"

#include "../src/Controllers/MainController.h"

TestBallProcessor::TestBallProcessor() : ballProcessor_(std::make_unique<BallProcessor>()), ballCounter_(0),
                                         eggCounter_(0)
{
}

void TestBallProcessor::begin()
{
  ballCounter_ = 0;
  eggCounter_ = 0;
  ballProcessor_->begin();
}

bool TestBallProcessor::isBallValid(CourseObject* courseObject)
{
  bool result = ballProcessor_->isBallValid(courseObject);
  if (result)
  {
    ballCounter_++;
  }
  return result;
}

bool TestBallProcessor::isEggValid(const CourseObject* courseObject)
{
  bool result = ballProcessor_->isEggValid(courseObject);
  if (result)
  {
    eggCounter_++;
  }
  return result;
}

int TestBallProcessor::getBallCounter()
{
  return ballCounter_;
}

int TestBallProcessor::getEggCounter()
{
  return eggCounter_;
}

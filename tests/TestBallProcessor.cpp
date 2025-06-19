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
}

bool TestBallProcessor::isBallValid(CourseObject* courseObject)
{
  if (ballProcessor_->isBallValid(courseObject))
  {
    ballCounter_++;
  }
}

bool TestBallProcessor::isEggValid(const CourseObject* courseObject)
{
  if (ballProcessor_->isEggValid(courseObject))
  {
    eggCounter_++;
  }
}

int TestBallProcessor::getBallCounter()
{
  return ballCounter_;
}

int TestBallProcessor::getEggCounter()
{
  return eggCounter_;
}

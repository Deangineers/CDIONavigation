//
// Created by Elias Aggergaard Larsen on 25/05/2025.
//

#include "Ball.h"

#include "../TextureLocations.h"

Ball::Ball(EngineBase* engineBase) : ball_(std::make_shared<DrawAble>())
{
  engineBase->registerDrawAble(ball_);
  ball_->setSize(20,20);
  ball_->setTextureLocation(&ball);
  ball_->setPosition(-100,-100);
}

void Ball::addToNavController(NavigationController* navController) const
{
  int x1 = ball_->getX();
  int y1 = ball_->getY();
  int x2 = ball_->getX() + 20;
  int y2 = ball_->getY() + 20;

  if (x1 > 0 && x2 > 0)
  {
    navController->addCourseObject(std::make_unique<CourseObject>(x1,y1,x2,y2,"ball"));
  }

}

void Ball::handleMouseClick(std::pair<int, int> mousePos)
{
  ball_->setPosition(mousePos.first, mousePos.second);
}

int Ball::getX() const
{
  return ball_->getX();
}

int Ball::getY() const
{
  return ball_->getY();
}

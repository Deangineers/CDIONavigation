//
// Created by Emil Damholt Savino on 25/05/2025.
//

#include "Egg.h"
#include "../TextureLocations.h"
Egg::Egg(EngineBase* engineBase) : egg_(std::make_shared<DrawAble>())
{
  engineBase->registerDrawAble(egg_);
  egg_->setSize(50,50);
  egg_->setTextureLocation(&egg);
  egg_->setPosition(-100,-100);
}

void Egg::addToNavController(NavigationController* navController) const
{
  int x1 = egg_->getX();
  int x2 = egg_->getX();
  int y1 = egg_->getY();
  int y2 = egg_->getY();

  if (x1 > 0 && x2 > 0)
  {
    navController->addCourseObject(std::make_unique<CourseObject>(x1,y1,x2,y2,"egg"));
  }
}

void Egg::handleMouseClick(std::pair<int,int> mousePos)
{
  egg_->setPosition(mousePos.first, mousePos.second);
}

int Egg::getX() const
{
  return egg_->getX();
}

int Egg::getY() const
{
  return egg_->getY();
}


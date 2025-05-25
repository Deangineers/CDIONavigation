//
// Created by Emil Damholt Savino on 25/05/2025.
//

#include "Cross.h"

#include "../TextureLocations.h"

Cross::Cross(EngineBase* engineBase) : cross_(std::make_shared<DrawAble>()) {
  engineBase->registerDrawAble(cross_);
  cross_->setSize(200,200);
  cross_->setTextureLocation(&cross);
  cross_->setPosition(500,300);
}

void Cross::addToNavController(NavigationController* navController) const {
  int x1 = cross_->getX();
  int y1 = cross_->getY();
  int x2 = cross_->getX() + 200;
  int y2 = cross_->getY() + 200;

  if (x1 > 0 && x2 > 0) {
    navController->addCourseObject(std::make_unique<CourseObject>(x1,y1,x2,y2,"blockedObject"));
  }
}

int Cross::getX() const {
  return cross_->getX();
}

int Cross::getY() const {
  return cross_->getY();
}
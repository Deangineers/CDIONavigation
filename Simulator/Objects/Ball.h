//
// Created by Elias Aggergaard Larsen on 25/05/2025.
//

#ifndef BALL_H
#define BALL_H
#include <memory>

#include "../../src/Controllers/NavigationController.h"
#include "EngineBase/DrawAble.h"
#include "EngineBase/EngineBase.h"


class Ball
{
public:
  explicit Ball(EngineBase* engineBase);
  void addToNavController(NavigationController* navController) const;
  [[nodiscard]] int getX() const;
  [[nodiscard]] int getY() const;
private:
  std::shared_ptr<DrawAble> ball_;
};



#endif //BALL_H

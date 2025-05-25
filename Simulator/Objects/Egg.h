//
// Created by Emil Damholt Savino on 25/05/2025.
//

#ifndef EGG_H
#define EGG_H

#include <memory>

#include "../../src/Controllers/NavigationController.h"
#include "EngineBase/DrawAble.h"
#include "EngineBase/EngineBase.h"


class Egg {
public:
  explicit Egg(EngineBase* engineBase);
  void addToNavController(NavigationController* navController) const;
  [[nodiscard]] int getX() const;
  [[nodiscard]] int getY() const;
  void handleMouseClick(std::pair<int,int> mousePos);
private:
  std::shared_ptr<DrawAble> egg_;
};



#endif //EGG_H

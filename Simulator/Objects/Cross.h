//
// Created by Emil Damholt Savino on 25/05/2025.
//

#ifndef CROSS_H
#define CROSS_H
#include <memory>

#include "../../src/Controllers/NavigationController.h"
#include "EngineBase/DrawAble.h"
#include "EngineBase/EngineBase.h"

class Cross {
public:
  explicit Cross(EngineBase* engineBase);
  void addToNavController(NavigationController* navController) const;
  [[nodiscard]] int getX() const;
  [[nodiscard]] int getY() const;
private:
  std::shared_ptr<DrawAble> cross_;
};



#endif //CROSS_H

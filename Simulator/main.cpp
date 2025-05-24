#include <iostream>
#include <ostream>

#include "EngineBase/EngineBase.h"
//
// Created by Elias Aggergaard Larsen on 24/05/2025.
//
int main(int argc, char* argv[])
{
  auto engineBase = std::make_shared<EngineBase>();
  engineBase->launch();
}

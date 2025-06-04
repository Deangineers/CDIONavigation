//
// Created by Elias Aggergaard Larsen on 24/05/2025.
//
#include "Simulator.h"
#include "Utility/ConfigController.h"

int main(int argc, char* argv[])
{
  ConfigController::loadConfig("../../config.json");
  Simulator();
}

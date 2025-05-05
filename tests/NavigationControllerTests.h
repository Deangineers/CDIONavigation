//
// Created by eal on 5/3/25.
//

#ifndef NAVIGATIONCONTROLLERTESTS_H
#define NAVIGATIONCONTROLLERTESTS_H
#include <vector>

#include "../src/Controllers/MainController.h"
#include "../src/Controllers/NavigationController.h"
#include "gtest/gtest.h"


class NavigationControllerTests : public ::testing::Test
{

};


TEST(NavControllertests, NavControllerTest1)
{
  auto navController = std::make_unique<NavigationController>();
  navController->addCourseObject(std::make_unique<CourseObject>(1,0,1,0,"ball"));
  navController->addCourseObject(std::make_unique<CourseObject>(0,0,0,0,"robotFront"));
  navController->addCourseObject(std::make_unique<CourseObject>(0,-1,0,-1,"robotBack"));
  auto journey = navController->calculateDegreesAndDistanceToObject();
  auto command = MainController::journeyToCommand(journey.get());
  EXPECT_EQ(command.getAction(), "r");

}


#endif //NAVIGATIONCONTROLLERTESTS_H

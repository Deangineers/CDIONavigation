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
  EXPECT_EQ(journey->angle, 90.0);
  EXPECT_EQ(journey->distance, 1.0);
  EXPECT_EQ(journey->collectBalls, true);
  auto command = MainController::journeyToCommand(journey.get());
  EXPECT_EQ(command.getAction(), "r");
  EXPECT_EQ(command.getMotor(), "");
  EXPECT_EQ(command.getSpeed(), 500);

  // B = ball
  // b = robotBack
  // f = robotFront
  // . . . . .
  // . . f B .
  // . . b . .
}

TEST(NavControllertests, NavControllerTest2)
{
  auto navController = std::make_unique<NavigationController>();
  navController->addCourseObject(std::make_unique<CourseObject>(-1,0,-1,0,"ball"));
  navController->addCourseObject(std::make_unique<CourseObject>(0,-1,0,-1,"robotBack"));
  navController->addCourseObject(std::make_unique<CourseObject>(0,0,0,0,"robotFront"));
  auto journey = navController->calculateDegreesAndDistanceToObject();
  EXPECT_EQ(journey->angle, 90);
  EXPECT_EQ(journey->distance, 1.0);
  EXPECT_EQ(journey->collectBalls, true);
  auto command = MainController::journeyToCommand(journey.get());
  EXPECT_EQ(command.getAction(), "r");
  EXPECT_EQ(command.getMotor(), "");
  EXPECT_EQ(command.getSpeed(), 500);

  // B = ball
  // b = robotBack
  // f = robotFront
  // . . b . .
  // . B f . .
  // . . . . .
}

TEST(NavControllertests, NavControllerTest3)
{
  auto navController = std::make_unique<NavigationController>();
  navController->addCourseObject(std::make_unique<CourseObject>(2,0,2,0,"ball"));
  navController->addCourseObject(std::make_unique<CourseObject>(0,0,0,0,"robotBack"));
  navController->addCourseObject(std::make_unique<CourseObject>(1,0,1,0,"robotFront"));
  auto journey = navController->calculateDegreesAndDistanceToObject();
  EXPECT_EQ(journey->angle, 0);
  EXPECT_EQ(journey->distance, 1.0);
  EXPECT_EQ(journey->collectBalls, true);
  auto command = MainController::journeyToCommand(journey.get());
  EXPECT_EQ(command.getAction(), "f");
  EXPECT_EQ(command.getMotor(), "");
  EXPECT_EQ(command.getSpeed(), 100);

  // B = ball
  // b = robotBack
  // f = robotFront
  // . . . . .
  // . f b B .
  // . . . . .
}

TEST(NavControllertests, NavControllerTest4)
{
  auto navController = std::make_unique<NavigationController>();
  navController->addCourseObject(std::make_unique<CourseObject>(2,2,2,2,"ball"));
  navController->addCourseObject(std::make_unique<CourseObject>(0,0,0,0,"robotBack"));
  navController->addCourseObject(std::make_unique<CourseObject>(1,1,1,1,"robotFront"));
  auto journey = navController->calculateDegreesAndDistanceToObject();
  EXPECT_EQ(static_cast<int>(journey->angle), 0);
  EXPECT_EQ(journey->distance, sqrt(2));
  EXPECT_EQ(journey->collectBalls, true);
  auto command = MainController::journeyToCommand(journey.get());
  EXPECT_EQ(command.getAction(), "f");
  EXPECT_EQ(command.getMotor(), "");
  EXPECT_EQ(command.getSpeed(), 100);

  // B = ball
  // b = robotBack
  // f = robotFront
  // . . . B .
  // . . f . .
  // . b . . .
}


#endif //NAVIGATIONCONTROLLERTESTS_H

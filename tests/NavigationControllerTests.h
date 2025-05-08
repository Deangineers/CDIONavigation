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
  navController->addCourseObject(std::make_unique<CourseObject>(2,1,2,1,"ball"));
  navController->addCourseObject(std::make_unique<CourseObject>(0,1,0,1,"robotFront"));
  navController->addCourseObject(std::make_unique<CourseObject>(0,0,0,0,"robotBack"));
  auto journey = navController->calculateDegreesAndDistanceToObject();
  EXPECT_EQ(journey->angle, 90.0);
  EXPECT_EQ(journey->distance, 2.0);
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
  navController->addCourseObject(std::make_unique<CourseObject>(0,0,0,0,"ball"));
  navController->addCourseObject(std::make_unique<CourseObject>(1,1,1,1,"robotBack"));
  navController->addCourseObject(std::make_unique<CourseObject>(1,0,1,0,"robotFront"));
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

TEST(NavControllerstests, NavControllerTest5)
{
  auto navController = std::make_unique<NavigationController>();
  navController->addCourseObject(std::make_unique<CourseObject>(0,0,0,0,"robotBack"));
  navController->addCourseObject(std::make_unique<CourseObject>(0, 1, 0, 1, "robotFront"));
  navController->addCourseObject(std::make_unique<CourseObject>(40, 25, 40, 25, "ball"));
  auto journey = navController->calculateDegreesAndDistanceToObject();
  EXPECT_EQ(static_cast<int>(journey->angle), 59);
  EXPECT_EQ(static_cast<int>(journey->distance), 46);
  EXPECT_EQ(journey->collectBalls, true);
  auto command = MainController::journeyToCommand(journey.get());
  EXPECT_EQ(command.getAction(), "r");
  EXPECT_EQ(command.getMotor(), "");
  EXPECT_EQ(command.getSpeed(), 500);
}

TEST(NavControllerstests, NavControllerTest6)
{
  auto navController = std::make_unique<NavigationController>();
  navController->addCourseObject(std::make_unique<CourseObject>(0, 0, 0, 0, "ball"));
  navController->addCourseObject(std::make_unique<CourseObject>(2, 0, 2, 0, "robotFront"));
  navController->addCourseObject(std::make_unique<CourseObject>(1, 0, 1, 0, "robotBack"));
  auto journey = navController->calculateDegreesAndDistanceToObject();
  EXPECT_EQ(static_cast<int>(journey->angle), 180);
  EXPECT_EQ(static_cast<int>(journey->distance), 2);
  auto command = MainController::journeyToCommand(journey.get());
  EXPECT_EQ(command.getAction(), "r");
  EXPECT_EQ(command.getMotor(), "");
  EXPECT_EQ(command.getSpeed(), 500);
}

TEST(NavControllerstests, NavControllerTest7)
{
  auto navController = std::make_unique<NavigationController>();
  navController->addCourseObject(std::make_unique<CourseObject>(2, 3, 2, 3, "ball"));
  navController->addCourseObject(std::make_unique<CourseObject>(0, 1, 0, 1, "robotFront"));
  navController->addCourseObject(std::make_unique<CourseObject>(0, 0, 0, 0, "robotBack"));
  auto journey = navController->calculateDegreesAndDistanceToObject();
  EXPECT_EQ(static_cast<int>(journey->angle), 45);
  EXPECT_EQ(static_cast<int>(journey->distance), static_cast<int>(std::sqrt(8)));
  auto command = MainController::journeyToCommand(journey.get());
  EXPECT_EQ(command.getAction(), "r");
  EXPECT_EQ(command.getMotor(), "");
  EXPECT_EQ(command.getSpeed(), 500);

  // B = ball
  // b = robotBack
  // f = robotFront
  // . . . B .
  // . . . . .
  // . f . . .
  // . b . . .
}

TEST(NavControllertests, NavControllerTest8)
{
  auto navController = std::make_unique<NavigationController>();
  navController->addCourseObject(std::make_unique<CourseObject>(2, 3, 2, 3, "ball"));
  navController->addCourseObject(std::make_unique<CourseObject>(4, 1, 4, 1, "robotFront"));
  navController->addCourseObject(std::make_unique<CourseObject>(4, 0, 4, 0, "robotBack"));
  auto journey = navController->calculateDegreesAndDistanceToObject();
  EXPECT_EQ(static_cast<int>(journey->angle), -45);
  EXPECT_EQ(static_cast<int>(journey->distance), static_cast<int>(std::sqrt(8)));
  auto command = MainController::journeyToCommand(journey.get());
  EXPECT_EQ(command.getAction(), "l");
  EXPECT_EQ(command.getMotor(), "");
  EXPECT_EQ(command.getSpeed(), 500);

  // B = ball
  // b = robotBack
  // f = robotFront
  // B . . .
  // . . . .
  // . . f .
  // . . b .
}

TEST(NavControllertests, NavControllerTest9)
{
  auto navController = std::make_unique<NavigationController>();
  navController->addCourseObject(std::make_unique<CourseObject>(2, 3, 2, 3, "ball"));
  navController->addCourseObject(std::make_unique<CourseObject>(4, 5, 4, 1, "robotFront"));
  navController->addCourseObject(std::make_unique<CourseObject>(4, 6, 4, 0, "robotBack"));
  auto journey = navController->calculateDegreesAndDistanceToObject();
  EXPECT_EQ(static_cast<int>(journey->angle), 45);
  EXPECT_EQ(static_cast<int>(journey->distance), static_cast<int>(std::sqrt(8)));
  auto command = MainController::journeyToCommand(journey.get());
  EXPECT_EQ(command.getAction(), "r");
  EXPECT_EQ(command.getMotor(), "");
  EXPECT_EQ(command.getSpeed(), 500);

  // B = ball
  // b = robotBack
  // f = robotFront
  // . . b .
  // . . f .
  // . . . .
  // B . . .
}

#endif //NAVIGATIONCONTROLLERTESTS_H

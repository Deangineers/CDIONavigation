//
// Created by Elias Aggergaard Larsen on 01/05/2025.
//

#include "NavigationController.h"

std::string NavigationController::navigate()
{

  return "null";
}

void NavigationController::addCourseObject(std::unique_ptr<CourseObject>&& courseObject)
{
  switch (courseObject->name())
  {
    case "ball":
      {
        ballVector_.push_back(std::move(courseObject));
      }
    case "egg":
      {
        eggVector_.push_back(std::move(courseObject));
        break;
      }
    case "goal":
      {
        goal_ = std::move(courseObject);
        break;
      }
    case "robotFront":
      {
        robotFront_ = std::move(courseObject);
        break;
      }
      case "robotBack":
      {
        robotBack_ = std::move(courseObject);
        break;
      }
      default:
        break;
  }
}

void NavigationController::clearObjects()
{
  ballVector_.clear();
  eggVector_.clear();
  goal_ = nullptr;
  robotFront_ = nullptr;
  robotBack_ = nullptr;
}

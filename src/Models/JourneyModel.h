//
// Created by Emil Damholt Savino on 04/05/2025.
//

#ifndef TRAVELMODEL_H
#define TRAVELMODEL_H

struct JourneyModel {
  JourneyModel(double dist, double angl, bool collectB) : distance(dist), angle(angl), collectBalls(collectB)
  {}
  double distance;
  double angle;
  bool collectBalls;
};

#endif //TRAVELMODEL_H

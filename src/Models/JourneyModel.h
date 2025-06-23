//
// Created by Emil Damholt Savino on 04/05/2025.
//

#ifndef TRAVELMODEL_H
#define TRAVELMODEL_H

struct JourneyModel {
  JourneyModel(double dist, double angl, bool collectB) : distance(dist), angle(angl), collectBalls(collectB){}
  JourneyModel(double dist, double angl, bool collectB, bool isCross) : distance(dist), angle(angl), collectBalls(collectB), isCross(isCross)
  {}
  double distance;
  double angle;
  bool collectBalls;
  bool isCross = false;
};

#endif //TRAVELMODEL_H

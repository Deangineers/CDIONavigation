//
// Created by Emil Damholt Savino on 04/05/2025.
//

#ifndef TRAVELMODEL_H
#define TRAVELMODEL_H

struct JourneyModel {
  JourneyModel(double dist, double angl, bool collectB) : distance(dist), angle(angl), collectBalls(collectB){}
  JourneyModel(double dist, double angl, bool collectB, bool isCross) : distance(dist), angle(angl), collectBalls(collectB), isCross(isCross)
  {}
  JourneyModel(double dist, double angl, bool collectB, bool isCross, bool intermediate) : distance(dist), angle(angl), collectBalls(collectB), isCross(isCross),isIntermediate(intermediate)
  {}
  JourneyModel(double dist, double angl, bool collectB, bool isCross, bool intermediate, bool isSafeSpot) : distance(dist), angle(angl), collectBalls(collectB), isCross(isCross),isIntermediate(intermediate), isSafeSpot(isSafeSpot)
  {}
  double distance;
  double angle;
  bool collectBalls;
  bool isCross = false;
  bool isIntermediate = false;
  bool isSafeSpot = false;
};

#endif //TRAVELMODEL_H

//
// Created by Emil Damholt Savino on 04/05/2025.
//

#ifndef TRAVELMODEL_H
#define TRAVELMODEL_H

enum BallCollectionMotorMode {
  cout, cin, off
};

struct JourneyModel {
  double distance;
  double angle;
  BallCollectionMotorMode ballCollectionMotorMode;
};

#endif //TRAVELMODEL_H

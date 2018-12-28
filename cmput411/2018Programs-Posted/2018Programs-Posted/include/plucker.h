/*
Author:  Herb Yang, Oct. 22, 2018
Purpose:
Define plucker coordinate
*/
#ifndef PLUCKER_H
#define PLUCKER_H
#include "vector3.h"
class Plucker {
public:
    Vector3 d;  //direction
    Vector3 m; // moment
// Constructor
    Plucker();
    Plucker(Vector3 d, Vector3 p);// d is the direction, p is the point that the line goes through
    double operator% (const Plucker& v) const;
};

#endif // PLUCKER_H

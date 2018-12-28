/*
Author: Herb Yang, Oct. 22, 2018
*/

#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "color.h"
#include "mathtypes.h"
#include "vector3.h"

class PointLight {
public:
    Color col;			// color of the light
    Vector3 posn;			// position
};


#endif

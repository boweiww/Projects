/*
Author:  Herb Yang, Oct. 22, 2018
Purpose:
Define plucker coordinate
only the permutted inner product % is defined
*/
#include "plucker.h"
Plucker::Plucker(){
    d.x = d.y = d.z = 0.0;
    m.x = m.y = m.z = 0.0;
}

Plucker::Plucker(Vector3 a, Vector3 b){
	this->d = a;
	this->m = a.cross(b);
	// complete this constructor

}
// dot product
double Plucker::operator% (const Plucker& v) const{
	double a = 0.0;
	// complete this function
	Vector3 tempm = Plucker::m;
	Vector3 tempd = Plucker::d;
	Vector3 b = v.m;
	a = tempm.dot(v.d) + b.dot(tempd);
	return a;
}

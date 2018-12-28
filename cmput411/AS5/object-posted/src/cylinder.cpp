/*
Author:  Herb Yang Oct. 22, 2018

Scene Object: Cylinder
*/
#include<iostream>
#include <math.h>
#include "cylinder.h"
#include <stdio.h>
#define min(a, b) (((a) < (b)) ? (a) : (b)) 
#define max(a, b) (((a) > (b)) ? (a) : (b)) 
// ========================================================================
//     getNormal of cylinder
// ========================================================================

// Return the normal vector for the given point on the surface
Vector3 Cylinder::getNormal(Vector3 *point) {
    Vector3 norm;// normal of the cylinder
	//
    // complete this function
	//
	if ((c1 - *point).dot(c1 - c1) == 0 || (c2 - *point).dot(c1 - c1) == 0) {
		norm = c1 - c2;
	}
	//double sint, cost, r;
	//Vector3 np;
	//sint = rad / (*point - c1).length();
	//cost = sqrt(1 - pow(sint, 2));
	//r = cost * (*point - c1).length() / (c1 - c2).length();
	//np = c1 + (c2 - c1)*r;
	//norm = *point - np;
	else{

		double m;
		Vector3 l = c2 - c1;

		m = point->dot(l) - c1.dot(l);
		norm = *point - c1 - m * l;
	}

    return norm;
} // getNormal
// ========================================================================
//     return true if the cylinder is specular
// ========================================================================
bool Cylinder::isSpecular()
{
    return( specular.r>0 || specular.g>0 || specular.b>0);
}
bool Cylinder::isDiffuse()
{
    return( diffuse.r>0 || diffuse.g>0 || diffuse.b>0);
}

// ========================================================================
//    Cylidner Intersection
// ========================================================================


// Find and return, in k, the earliest intersection point, in the
// range [k0,k1), along the given ray. Returns 1 if such a k exists,
// false otherwise
bool Cylinder::intersect(RTfloat *k,  Ray *ray, RTfloat k0,  RTfloat k1)
{
	bool flag = false; // intersection flag
					   // complete this function
	Vector3 l = c2 - c1,temp1,temp2;
	double r1,t1,t2,tt,m1,m2,temppp;
	double ee = 0.01;
	l.normalize();
	temp1 = (ray->o - c1) - ((ray->o.dot(l) - c1.dot( l))*l);
	temp2 = ray->d - (ray->d.dot(l)*l);
	
	r1 = pow((temp2.dot(temp1) + temp1.dot(temp2)), 2) - 4* (temp1.dot(temp1) - pow(rad,2))*temp2.dot(temp2);
	if (r1 < 0) {
		return flag;
	}
	t2 = (sqrt(r1) - (temp2.dot(temp1) + temp1.dot(temp2))) / (2 * temp2.dot(temp2));
	t1 = (-sqrt(r1) - (temp2.dot(temp1) + temp1.dot(temp2))) / (2 * temp2.dot(temp2));

	if (t1 > t2) {
		tt = t1;
		t1 = t2;
		t2 = tt;
		//make sure t1 < t2
	}
	//cout << t1<<"\n";
	if (fabs(t1) < ee) {
		//m = O · l + tk · l − C1 · l
		m2 = ray->o.dot(l) + t2* ray->d.dot(l) - c1.dot(l);
		m1 = ray->o.dot(l) + t1 * ray->d.dot(l) - c1.dot(l);

		if ((m1 >= 0 && m1 <= (c2-c1).length()) || (m2 >= 0 && m2 <= (c2 - c1).length())){
			*k = t1;
			outside = false;
			return true;
		}
	}
	else if (t1 > k0 && t1 < k1) {
		m2 = ray->o.dot(l) + t2 * ray->d.dot(l) - c1.dot(l);
		m1= ray->o.dot(l) + t1 * ray->d.dot(l) - c1.dot(l);

		if ((m1 >= 0 && m1 <= (c2 - c1).length()) || (m2 >= 0 && m2 <= (c2 - c1).length())) {
			*k = t1;
			outside = true;
			return true;
		}

	}
	else if (t2 > k0 && t2 <k1 && !solidFlag) {
		m2 = ray->o.dot(l) + t2 * ray->d.dot(l) - c1.dot(l);

		m1 = ray->o.dot(l) + t1 * ray->d.dot(l) - c1.dot(l);
		if ((m1 >= 0 && m1 <= (c2 - c1).length()) || (m2 >= 0 && m2 <= (c2 - c1).length())) {
			*k = t2;
			outside = false;
			return true;
		}

	}
	return flag;
} // intersect()
// getDiffuse() - return the diffuse coefficient
Color Cylinder::getDiffuse() {
    return diffuse;
}
// getSpecular() - return the specular coefficient
Color Cylinder::getSpecular() {
    return specular;
}
// getDiffuse() - return the ambient coefficient
Color Cylinder::getAmbient() {
    return ambient;
}
// getSpecularExp() - return the Specular Exponent coefficient
RTfloat Cylinder::getSpecularExp() {
    return specularExp;
}
// set the attributes of the cylinder
// The order of the attributes is given below:
// rad: radius
// center1.x
// center1.y
// center1.z
// center2.x
// center2.y
// center2.z
// solidFlag
// diffuse.r
// diffuse.g
// diffuse.b
// ambient.r
// ambient.g
// ambient.b
// specular.r
// specular.g
// specular.b
// specularExp
void Cylinder::setAttributes(vector<double> a) {
    rad         = a[0];
    c1.x        = a[1];
    c1.y        = a[2];
    c1.z        = a[3];
    c2.x        = a[4];
    c2.y        = a[5];
    c2.z        = a[6];
    solidFlag   = a[7];
    diffuse.r   = a[8];
    diffuse.g   = a[9];
    diffuse.b   = a[10];
    ambient.r   = a[11];
    ambient.g   = a[11];
    ambient.b   = a[12];
    specular.r  = a[14];
    specular.g  = a[15];
    specular.b  = a[16];
    specularExp = a[17];
}


/*
Author:  Herb Yang, Oct. 22, 2018
Purpose:
Scene Object: Polygon
The polygon is specified by n vertices, p1, p2, ...
*/

#include <math.h>
#include "poly.h"
#include "vector3.h"
#include "plucker.h"


// ========================================================================
//     getNormal of polygon
// ========================================================================

// Return the normal vector for the given point on the surface
// The vertices are specified in CCW direction.
//
// point is here because to be consistent with other objects.
Vector3 Poly::getNormal(Vector3 *point) {


	return normal;// already computed when the attributes are set
} // getNormal
// ========================================================================
//     return true if specular
// ========================================================================
bool Poly::isSpecular()
{
	return(specular.r > 0 || specular.g > 0 || specular.b > 0);
}
// ========================================================================
//     return true if diffuse
// ========================================================================
bool Poly::isDiffuse()
{
	return(diffuse.r > 0 || diffuse.g > 0 || diffuse.b > 0);
}

// ========================================================================
//    Plucker test to check if the ray goes through the object
// ========================================================================
bool Poly::pluckerTest(Ray *ray) {
	bool flag = false;
	// complete this function
	Vector3 vert, nor, temp, v1, v2;
	int mark = 0, num = 0;
	RTfloat tempt;
	Plucker p0, p1, p2, p3;
	double a1, a2, a3;
	for (int i = 0; i < nVertices; i++) {
		for (int j = 0; j < nVertices; j++) {
			for (int k = 0; k < nVertices; k++) {
				if (i == j || j == k || i == k) {
					continue;
				}
				vert = Poly::vertices[i];

				v1 = Poly::vertices[j];
				v2 = Poly::vertices[k];


				temp = (vert + v1 + v2) / 3;

				nor = getNormal(&temp);
				p0 = Plucker(ray->d, ray->o);

				p1 = Plucker((v2 - vert), vert);
				p2 = Plucker((vert - v1), v1);
				p3 = Plucker((v1 - v2), v2);

				a1 = p0.operator% (p1);
				a2 = p0.operator% (p2);
				a3 = p0.operator% (p3);

				if ((a1 > 0 && a2 > 0 && a3 > 0) || (a1 < 0 && a2 < 0 && a3 < 0)) {
					return true;
				}
			}
		}
	}
	return false;
}

// Find and return, in t, the earliest intersection point, in the
// range [t0,t1), along the given ray. Returns 1 if such a t exists,
// false otherwise
bool Poly::intersect(RTfloat *t, Ray *ray, RTfloat t0, RTfloat t1)
{
	bool flag = false;
	Vector3 vert, nor, temp,v1,v2;
	int mark = 0, num = 0;
	RTfloat tempt,ttt;
	Plucker p0,p1,p2,p3;
	double a1, a2, a3;
	// complete this function
	if (!pluckerTest(ray) ){
		return flag;
	}
	ttt = 9999999999;

	for (int i = 0; i < nVertices; i++) {
		for (int j = 0; j < nVertices; j++) {
			for (int k = 0; k < nVertices; k++) {
				if (i == j || j == k || i == k) {
					continue;
				}
				vert = Poly::vertices[i];

				v1 = Poly::vertices[j];
				v2 = Poly::vertices[k];


			}
			temp = (vert + v1 + v2) / 3;

			nor = getNormal(&temp);
			tempt = (vert - ray->o).dot(nor) / ray->d.dot(nor);
			if (tempt < ttt &&  tempt > t0 && tempt < t1) {
				ttt = tempt;
				*t = ttt;
				flag = true;
			}
		}
	}
		


	return flag;
} // intersect()
// getDiffuse() - return the diffuse coefficient
Color Poly::getDiffuse() {
    return diffuse;
}
// getSpecular() - return the specular coefficient
Color Poly::getSpecular() {
    return specular;
}
// getDiffuse() - return the ambient coefficient
Color Poly::getAmbient() {
    return ambient;
}
// getSpecularExp() - return the Specular Exponent coefficient
RTfloat Poly::getSpecularExp() {
    return specularExp;
}
// set the attributes of the polygon
// The order of the attributes is given below:
// nVertices
// vertices[0].x, vertices[0].y, vertices[0].z
// vertices[1].x, vertices[1].y, vertices[1].z
// ...
// vertices[nVertices-1].x, vertices[nVertices-1].y, vertices[nVertices-1].z
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
void Poly::setAttributes(vector<double> a) {
    int i = 0;
    nVertices =a[i++];
    vertices = new Vector3[nVertices];
    for(int j=0;j<nVertices;j++){
        vertices[j].x = a[i++];
        vertices[j].y = a[i++];
        vertices[j].z = a[i++];
    }
    diffuse.r = a[i++];
    diffuse.g = a[i++];
    diffuse.b = a[i++];
    ambient.r = a[i++];
    ambient.g = a[i++];
    ambient.b = a[i++];
    specular.r = a[i++];
    specular.g = a[i++];
    specular.b = a[i++];
    specularExp = a[i++];
    Vector3 aa = vertices[1]-vertices[0];
    Vector3 bb = vertices[nVertices-1]-vertices[0];
    normal = aa*bb; // cross product
    normal.normalize();
}
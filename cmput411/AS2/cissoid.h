// Author: Herb Yang
// Date:   Sept. 20, 2018
// Creative Common License
#ifndef CISSOID_H
#define CISSOID_H
#include <GL/freeglut.h>
#include <math.h>
#include <vector>
#include <iostream>
#define BORDER 10
#define WIN_SIZE 300		// size of window
#define WIN_OFFSET 100		// offset of window
#define GRID_NUM 4			// default number of grids  - it is actually from -4 to +4
#define DOT_SIZE 5			// radius of the dots
#define PI 3.1415926535898
int _px;			// window size in pixels in x
int _py;			// window size in pixels in y
int _nx;			// grid size - the grid goes from -_nx to +_nx
int _ny;			// grid size - the grid goes from -_ny to +_ny
int k;              // f(x,y) = x^3 +(x-2k)y^2 = 0
GLfloat _br;		// background colour
GLfloat _bg;
GLfloat _bb;
using namespace std;
vector<GLuint> dots; // the id of the collection of dots - use this to store the dots of your curve
#endif
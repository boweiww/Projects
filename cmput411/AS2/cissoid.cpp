
//  AUTHOR:  Herb Yang
//  University of Alberta
//  September 20, 2018
//  CMPUT 411/511 Assignment 2

//bowei wang 1462495
#include "cissoid.h"
#include <list>

void gridToPixel(int gx, int gy, int& px, int& py)
{


        // add you code in here


	   
}
void drawGrid()
{	
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);

	int xnumber = _px / _nx;
	int ynumber = _py / _ny;
	for (int i = 1; i <= xnumber; i++)
	{
		glVertex2i(i*_nx,0 );
		glVertex2i(i*_nx, _py);

	}
	for (int j = 1; j <= ynumber; j++) {
		glVertex2i(0, j*_ny);
		glVertex2i(_px, j*_ny);
	}



	//draw a line


	glEnd();
	glColor3f(1.0, 0.0, 0.0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON);
	int x = xnumber / 2;
	int y = ynumber / 2;
	int radius = DOT_SIZE;
	float angle = 3.141592653 / 360;
	for (int i = 0; i < 720; i++)
	{
		float degInRad = i * angle;
		glVertex2f(cos(degInRad)*radius + x*_nx, sin(degInRad)*radius + y*_ny);
	}
	glEnd();
	glFlush();

	
	//add you code in here

}
void removeDots()
{
        // add your code in here
}
void drawCircularDot(int x, int y)
{
	glColor3f(1.0, 0.0, 0.0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_POLYGON); 

	int dotsize = 4;
	float angle = 3.141592653 / 360;
	for (int i = 0; i < 720; i++)
	{
		float degInRad = i * angle;
		glVertex2f(cos(degInRad)*dotsize + x, sin(degInRad)*dotsize + y);
	}
	glEnd();
	

	
        // add your code in here


}


void midPointCissoid(int x0,int y0) {

	int x, y, yt, xt, sit;
	float d1, d2, pred1, pred2;
	pred1 = 5 - 4 * k;
	pred2 = 5 / 2.0 - 4 * k;
	printf("%f%f", pred1, pred2);
	int total = (_py / _ny) * 2;
	x = 0;
	y = 0;
	sit = 0;
	yt = 0;
	xt = 0;
	for (int i = 0; i < total; i++) {
		if (pred1 < 0) {
			//choose B


			d1 = pred1 + 12 * pow(x, 2) + 36 * x + 4 * pow(y, 2) + 4 * y + 29;
			d2 = pred2 + 12 * pow(x, 2) + 36 * x + 4 * pow(y, 2) + 8 * y + 1;
			pred1 = d1;
			pred2 = d2;
			x = x + 1;

			drawCircularDot((x + x0)*_nx, (y + y0)*_ny);
			drawCircularDot((x + x0)*_nx, (y0 - y)*_ny);


		}
		else if (pred2 < 0) {
			d1 = pred1 + 12.0 * pow(x, 2.0) + 44.0 * x + 4.0 * pow(y, 2.0) + 20.0 * y + 8.0 * x*y - 32.0 * k*y + 45.0 - 32.0 * k;
			d2 = pred2 + 12.0 * pow(x, 2.0) + 36.0 * x + 8.0 * x*y + 4.0 * pow(y, 2.0) + (20.0 - 32.0 * k)*y + 35.0 - 48.0 * k;
			pred1 = d1;
			pred2 = d2;
			x = x + 1;
			y = y + 1;
			drawCircularDot((x + x0)*_nx, (y + y0)*_ny);
			drawCircularDot((x + x0)*_nx, (y0 - y)*_ny);


			//choose C


		}
		else {

			d2 = pred2 + 8 * y * x + 12 * x + (4 - 32 * k)*y + 6 - 48 * k;
			pred2 = d2;
			y = y + 1;
			drawCircularDot((x + x0)*_nx, (y + y0)*_ny);
			drawCircularDot((x + x0)*_nx, (y0 - y)*_ny);

			//choose D

		}
		
		glFlush();
		//printf("%d  %d\n", x, y);
		//printf("%f %f\n", pred1, pred2);
	}
}

void cissoid()
// f(x,y) = x^3 +(x-4k)y^2 = 0
{
	int x0 = _px / _nx / 2 ;
	int y0 = _py / _ny / 2 ;

	drawCircularDot(x0* _nx, y0* _ny);
	midPointCissoid(x0,y0);
	
	
	// add your code in here

}

void draw()
{  
    cissoid(); // draw a cissoid
}

//----------- Display:  The main display module ---------------------------- 
void display(void) 
{ 
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); 
    drawGrid(); 
    draw(); // draw the curve
	for(int i=0;i<(int) dots.size();i++)
    {
          glCallList(dots[i]);
    }
	glutSwapBuffers();
	glFlush(); 
} 

//--------- Mouse handler - not used ------------------------------------
void mouse(int button, int state, int x, int y)
{
	if(button==GLUT_LEFT_BUTTON && state ==GLUT_DOWN)
    {
		
		glutPostRedisplay();
	}
}
void keyPressed (unsigned char key, int x, int y){
	// add your keyboard control in here
	if (key == 'g' &&  _ny >= 5) {
		_nx = _nx - 1;
		_ny = _ny - 1;
	}
	else if (key == 'G') {
		_nx = _nx + 1;
		_ny = _ny + 1;
	}
	else if (key == 'k' && k >= 2) {
		k = k - 1;
	}

	else if (key == 'K' ) {
		k = k + 1;
	}


	
	glutPostRedisplay();
}
//--------------- Initialize:  Parameter initialization ------------------
void initialize(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0); 
	glPointSize(4.0); 
	glLineWidth(2.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WIN_SIZE, 0, WIN_SIZE);
	k = 3;  // initial value of k
	cout<<"k = "<<k<<"\n";
    _nx = 8;
    _ny = 8;
    _px = WIN_SIZE;
    _py = WIN_SIZE;
	_br =_bg = _bb =1.0;
	glClearColor(_br,_bg,_bb,1.0);
    glShadeModel(GL_SMOOTH);
}/*
The function gridToPixel(int gx, int gy, int& px, int& py) translates grid
coordinates(gx, gy) to pixel coordinates(px, py).The window size in pixels is defined by
(px, py) and the grid size by(nx, ny).There is also a border of size BORDER around the
window.
(b)(5 marks) The function drawGrid draws a grid defined by(nx, ny).
(c)(4 marks) Implement keyboard control with the following functions :
i.¡¯g¡¯ - decrease the number of grids by 1 with 4 as the minimum
ii.¡¯G¡¯ - increase the number of grids by 1
iii.¡¯k¡¯ - decrease k by 1 with a minimum value of 1
iv.¡¯K¡¯ - increase k by 1
(d)(5 marks) The function drawCircularDot(int x, int y) is to draw a red circular dot at grid
location(x, y).The radius is specified by DOT SIZE.
(e)(2 marks) The function removeDots() is to remove all the dots.
(f)(12 marks) The function midPointCissoid generates the required cissoid with parameter k*/
//  ------- Main: Initialize glut window and register call backs ---------- 
void main(int argc, char **argv) 
{ 
	glutInit(&argc, argv);            
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);  
	glutInitWindowSize(WIN_SIZE, WIN_SIZE);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Cissoid");
	initialize();
	glutDisplayFunc(display); 
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyPressed); 
	glutMainLoop(); 
}
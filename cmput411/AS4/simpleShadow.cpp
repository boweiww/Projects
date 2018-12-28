//  AUTHOR:  Herb Yang
//  Oct. 10, 2018
//  A program to demostrate the use of  projection to create shadow.
// Assignemnt 4 for cmput411
// Bowei Wang

#include <GL/freeglut.h>
#include <math.h>
#include <iostream>
#include <chrono>
#include <random>
GLfloat Lposition[] = { 0.0f, 60.0f, 0.0f };
GLfloat Cposition[] = { 0.0f, 30.0f, 0.0f };
int rotateN = 0;
//control if rotate or not, 0 not rotate , 1 rotate
int SWrotate = 0;
//control type, 1 for light source, 2 for box 
int controlType = 1;
using namespace std;


GLint screenSizeX = 500, screenSizeY = 500;
GLuint updateTime = 20;   // update time for the animation


// draw a plane with colour r, g, b, a - a is the alpha value
void drawPlane(GLfloat r, GLfloat g, GLfloat b, GLfloat a, GLfloat unit, GLfloat *val, int cases) {
	// add code
	// parallel to x-z plane
	if (cases == 1) {
		glColor4f(r, g, b, a);
		glBegin(GL_POLYGON);
		glVertex3f(val[0] + unit / 2, val[1], val[2] + unit / 2);
		glVertex3f(val[0] - unit / 2, val[1], val[2] + unit / 2);
		glVertex3f(val[0] - unit / 2, val[1], val[2] - unit / 2);
		glVertex3f(val[0] + unit / 2, val[1], val[2] - unit / 2);


		glEnd();
		glFlush();

	}
	// parallel to y-z plane

	else if (cases == 2) {
		glColor4f(r, g, b, a);
		glBegin(GL_POLYGON);
		glVertex3f(val[0], val[1] + unit / 2, val[2] + unit / 2);
		glVertex3f(val[0], val[1] - unit / 2, val[2] + unit / 2);
		glVertex3f(val[0], val[1] - unit / 2, val[2] - unit / 2);
		glVertex3f(val[0], val[1] + unit / 2, val[2] - unit / 2);


		glEnd();
		glFlush();

	}
	// parallel to x-y plane
	else if (cases == 3) {
		glColor4f(r, g, b, a);
		glBegin(GL_POLYGON);
		glVertex3f(val[0] + unit / 2, val[1] + unit / 2, val[2]);
		glVertex3f(val[0] - unit / 2, val[1] + unit / 2, val[2]);
		glVertex3f(val[0] - unit / 2, val[1] - unit / 2, val[2]);
		glVertex3f(val[0] + unit / 2, val[1] - unit / 2, val[2]);



		glEnd();
		glFlush();
	}


}
//----------- draw a cube with r, g, b, a------------
// each face has a colour which is a permutation of r, g, and b
//void colorGenerate(GLfloat r, GLfloat g, GLfloat b) {
//	r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
//	g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
//	b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
//}


void drawCube( GLfloat a, GLfloat *position, GLfloat unit,int type)
{
	////glColor3ub(rand() % 255, rand() % 255, rand() % 255);
	//std::random_device rd;  //Will be used to obtain a seed for the random number engine
	//std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	//std::uniform_real_distribution<> dis(0.0, 1.0);
	////dis(gen)

	if (type == 1) {
		GLfloat r = 0.3, g = 0.0, b = 1.0;
		//_sleep(10); // pauses 
		GLfloat val[] = { position[0], position[1] + unit / 2, position[2] };

		drawPlane(r, g, b, a, unit, val, 1);

		r = 1.0;
		val[1] = val[1] - unit;
		drawPlane(r, g, b, a, unit, val, 1);


		val[1] = position[1];
		val[0] = position[0] + unit / 2;
		r = 0.0;
		g = 1.0;
		drawPlane(r, g, b, a, unit, val, 2);


		val[0] = val[0] - unit;
		b = 0.0;
		drawPlane(r, g, b, a, unit, val, 2);

		val[0] = position[0];
		val[2] = position[2] + unit / 2;
		r = 1.0;
		drawPlane(r, g, b, a, unit, val, 3);

		val[2] = val[2] - unit;
		b = 0.5;
		drawPlane(r, g, b, a, unit, val, 3);

	}
	else if (type == 0) {
		GLfloat r = 0.0, g = 0.0, b = 0.0;
		GLfloat val[] = { position[0], position[1] + unit / 2, position[2] };

		drawPlane(r, g, b, a, unit, val, 1);

		val[1] = val[1] - unit;
		drawPlane(r, g, b, a, unit, val, 1);


		val[1] = position[1];
		val[0] = position[0] + unit / 2;
		drawPlane(r, g, b, a, unit, val, 2);


		val[0] = val[0] - unit;
		drawPlane(r, g, b, a, unit, val, 2);

		val[0] = position[0];
		val[2] = position[2] + unit / 2;
		drawPlane(r, g, b, a, unit, val, 3);

		val[2] = val[2] - unit;
		drawPlane(r, g, b, a, unit, val, 3);
	}

	// add code 
}
void rotate(int a) {
	// add code
		glTranslatef(Cposition[0], Cposition[1], Cposition[2]);

		if (a == 1) {
			rotateN += 1;
		}
		glRotatef(rotateN, 1.0, 1.0, 0.0);

		glTranslatef(-Cposition[0], -Cposition[1], -Cposition[2]);

		glutPostRedisplay();
		glutTimerFunc(updateTime, rotate, 0);

}



//----------- Display:  The main display module ---------------------------- 
void display(void)
{
	GLfloat m[] =//
	{ 1.0f,0,0,0,
		0,1.0f,0,-1.0f /(Lposition[1]-0.1),
		0,0,1.0f,0,
	0,0,0,0 };

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glMatrixMode(GL_MODELVIEW);
	GLdouble *temp;

	cout << m[13];
	glLoadIdentity(); 
	// add code 
	gluLookAt(0.0f, 100.0f, 100.0f, 0.0f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f);


	// draw plane
	//GLfloat position[] = { 0.0f, 30.0f, 0.0f };
	GLfloat val[] = { 0.0f, 0.0f, 0.0f };



	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	drawPlane(0.0f, 0.0f, 1.0f, 0.0f, 200.0f, val, 1);
	drawCube(1.0f, Lposition, 0.4f,1);



	
	//glOrtho(0.0, 1.0, 0.0, 1.0, 0.999, 1000);
	glLineWidth(5);
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(0.0, 1, -98);
	glVertex3f(0, 1, -300);
	glEnd();


	//glEnable(GL_BLEND);
	//glEnable(GL_STENCIL_TEST);
	//glEable(GL_DEPTH_TEST);

	glStencilFunc(GL_LEQUAL, 1, 0x0F);
	glStencilMask(0x00); // disable writing to the stencil buffer
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//glBlendFunc(GL_SRC_ALPHA,
	//	GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	/* Project the shadow. */
	//drawCube(1.0f, Cposition, 20.0f);
	glTranslatef(Lposition[0], Lposition[1], Lposition[2]);

	glMultMatrixf(m);


	glTranslatef(-Lposition[0], -Lposition[1], -Lposition[2]);


	//glTranslatef(Cposition[0], Cposition[1], Cposition[2]);
	rotate(SWrotate);

	drawCube(1.0f, Cposition, 20.0f,0);

	//glRotatef(rotateN, 1.0, 0.0, 0.0);
	//glTranslatef(-Cposition[0], -Cposition[1], -Cposition[2]);
	glPopMatrix();

	//glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);


	//glMatrixMode(temp);
	rotate(SWrotate);
	drawCube(1.0f, Cposition, 20.0f,1);





	// draw the light source
	
	// draw shadowF
	
	// draw box
	


	glFlush();
	glutSwapBuffers();
} 

//--------- Mouse call back function ------------------------------------
void mouse(int button, int state, int x, int y)
{
	if(button==GLUT_LEFT_BUTTON && state ==GLUT_DOWN)
	{
		
		glutPostRedisplay();
	}
}
// keyboard handler
void keyPressed (unsigned char key, int x, int y) { 
	// add code
	//control light source 
	if (key == 'l') {
		controlType = 3 - controlType;
	}
	else if (key == 'r') {
		SWrotate = 1 - SWrotate;
	}
	if (controlType == 1) {
		if (key == 'x') {
			Lposition[0] = Lposition[0] - 1;
		}
		else if(key == 'X') {
			Lposition[0] = Lposition[0] + 1;

		}
		else if (key == 'y') {
			Lposition[1] = Lposition[1] - 1;
		}
		else if (key == 'Y') {
			Lposition[1] = Lposition[1] + 1;
		}
		else if (key == 'z') {
			Lposition[2] = Lposition[2] - 1;
		}
		else if (key == 'Z') {
			Lposition[2] = Lposition[2] + 1;
		}
	}
	else if (controlType == 2) {
		if (key == 'x') {
			Cposition[0] = Cposition[0] - 1;
		}
		else if (key == 'X') {
			Cposition[0] = Cposition[0] + 1;

		}
		else if (key == 'y') {
			Cposition[1] = Cposition[1] - 1;
		}
		else if (key == 'Y') {
			Cposition[1] = Cposition[1] + 1;
		}
		else if (key == 'z') {
			Cposition[2] = Cposition[2] - 1;
		}
		else if (key == 'Z') {
			Cposition[2] = Cposition[2] + 1;
		}

	}

	glutPostRedisplay();
}

//--------------- Initialize:  Parameter initialization ------------------
void initialize(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0); 
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	// add code
}
void resizeGL(int width, int height)
{
	GLdouble aspect, theta, n, f, theta_radian, theta_2_radian, top, bott,left, right;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0,0,(GLsizei) width, (GLsizei) height);
	aspect = (GLdouble)width/(GLdouble)height;
	theta = 60.0; theta_radian = theta*3.1415926/180.0;
	theta_2_radian = theta_radian/2.0;
	n = 1.0;
	f = 1500.0;
	top = n*tan(theta_radian/2.0);
	bott = -top;
	right = top*aspect;
	left = -right;
	glFrustum(left,right, bott, top, n, f);
}

//  ------- Main: Initialize glut window and register call backs ---------- 
void main(int argc, char **argv) 
{ 
	glutInit(&argc, argv);            
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);  
	glutInitWindowSize(screenSizeX, screenSizeY);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Simple Shadow");
	initialize();
	glutDisplayFunc(display); 
	glutReshapeFunc(resizeGL);
	glutTimerFunc(updateTime, rotate, 0);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyPressed); 
	glutMainLoop(); 
}
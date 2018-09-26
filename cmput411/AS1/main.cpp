
//  AUTHOR:  Herb Yang
//  September 1, 2018



//#include <OpenGL/gl.h>
//
//#include <OpenGl/glu.h>
//
//#include <GLUT/glut.h>

#include <GL/freeglut.h>
#include "ball.h"
#include "box.h"
#include "block.h"
#include <time.h>
GLint screenSizeX = 400, screenSizeY = 600;	// screen size
GLfloat radius = 10;						// radius of ball
GLfloat updateTime = 10;
GLfloat totalTime = 0;
GLfloat maxTotalTime = 1000000;
GLint score = 0;
char s[10];
ball bouncingBall;
box wall(screenSizeY - 1.0f, 1.0f, 1.0f, screenSizeX - 1.0f, 0.0f, 1.0f, 0.0f, false);
block paddle(screenSizeX / 2.0f, screenSizeY / 8.0f, 30.0f, 80.0f, 0.0f, 0.5f, 0.5f, true);
// ---- display some text -----
void showText(GLfloat x, GLfloat y, char* s, GLfloat r, GLfloat g, GLfloat b) {
	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	while (*s) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *s);
		s++;
	}
}
//-------------- Display --------------
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();    
	snprintf(s, sizeof(s), "%d", score);
	showText(20.0f, 20.0f, "Score =", 0.0f, 0.0f, 1.0f);
	showText(100.0f, 20.0f, s, 0.0f, 0.0f, 1.0f);
	wall.draw();
	paddle.draw();
	bouncingBall.draw();
	glutSwapBuffers();
	glFlush();
}

//-------------- Mouse handler --------------
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		paddle.setLoc(x);// click to set the paddle location
		glutPostRedisplay();
	}
}

//-------------- Initialization --------------
void initialize(void)
{
	GLfloat x, y, incx, incy, r, g, b;
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, screenSizeX, 0, screenSizeY);
	srand(time(0));
	incx = (rand() % 5) + 1;			// random velocity of the ball
	incy = (rand() % 5) + 1;
	x = rand() % (screenSizeX)+radius;	// set random initial position
	y = rand() % (screenSizeY)+radius;
	bouncingBall.setLoc(x, y, radius, 1.0f, 0.0f, 0.0f);// set the location and colour of the ball
	bouncingBall.setDir(incx, incy);
}
//-------------- update --------------
void update(int value)
{
	totalTime += updateTime;				// update total time
	if (totalTime > maxTotalTime) return;
	bouncingBall.move();					// move the ball
	bouncingBall.collision(wall, &score);	// check for collision with the walls
	bouncingBall.collision(paddle, &score);	// check for collision with the paddle
    snprintf(s, sizeof(s), "%d", score);
	showText(100.0f, 20.0f, s, 0.0f, 0.0f, 1.0f);// display score
	glutPostRedisplay();					// update current window
	glutTimerFunc(updateTime, update, 0);
}
//-------------- Main --------------
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(screenSizeX, screenSizeY);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Pong");
	initialize();
	glutTimerFunc(updateTime, update, 0);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMainLoop();
}

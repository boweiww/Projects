#ifndef BALL_H
#define BALL_H
#include <GL/glut.h>

#include <iostream>
#include <math.h>
#include "box.h"
const double PI  =3.141592653589793238463;
class ball{
	GLfloat lastX, lastY;		// lastX and lastY
	GLfloat locX, locY, radius;// location and radius
	GLfloat r,g,b;	// ball colour
	GLfloat vX, vY; // ball direction
public:
	void setLoc(GLfloat x, GLfloat y, GLfloat rad, GLfloat red, GLfloat green, GLfloat blue){
		locX = x;
		locY = y;
		lastX = locX;
		lastY = locY;
		radius = rad;
		r = red;
		g = green;
		b = blue;
	};
	void setDir(GLfloat a, GLfloat b){
		vX = a;
		vY = b;
	};
	void move(){
	// move the ball forward by vX and vY. Need to keep track of last position
	// add code below
        lastX = locX;
        lastY = locY;
        locX = locX + vX;
        locY = locY + vY;


	// add code above
	};
	// inside is to return true if the point is inside the box
	// Note: the boundary must include the radius of the ball
	bool inside(GLfloat x, GLfloat y, GLfloat ty, GLfloat by, GLfloat lx, GLfloat rx) {
		if (x > lx && x <  rx && y> by && y<  ty) {
			return true;
		}
		else {
			return false;
		}
	}
	void collision(box b, GLint *score) {
		GLfloat pt;
		GLfloat pb;
		GLfloat pl;
		GLfloat pr;
		GLfloat t, newX, newY;
		if (b.isFilled()) {		// paddle
			pt = b.top() + radius;
			pb = b.bottom() - radius;
			pl = b.left() - radius;
			pr = b.right() + radius;
			if (inside(locX, locY, pt, pb, pl, pr))
			{
				// there are 4 cases
				// case 1: the ball is initially above the paddle and is going down
				// case 2: the ball is initially below the paddle and is going up
				// case 3: the case is initially to the left of the paddle and is going right
				// case 4: the ball is initially to the right of the paddle and is going left
				// Add code below
                if(locY <= pt && locY > b.top() && locX >= b.left() && locX <= b.right() && vY < 0){
                    vY = -vY;
                    *score += 1;
                }
                else if(locY >= pb && locY < b.bottom() && locX >= b.left() && locX <= b.right() && vY > 0){
                    vY = -vY;
                    *score += 1;

                }
                else if(locX <= pr && locX > b.right() && locY >= b.bottom() && locY <= b.top() && vX < 0){
                    vX = -vX;
                    *score += 1;
                }
                else if(locX >= pl && locX < b.left() && locY >= b.bottom() && locY <= b.top() && vX > 0){
                    vX = -vX;
                    *score += 1;

                }







				// add code above
			}
		}
		else { // box
			pt = b.top() - radius;
			pb = b.bottom() + radius;
			pl = b.left() + radius;
			pr = b.right() - radius;
			if (!inside(locX, locY, pt, pb, pl, pr)) {
				// there are 4 cases
				// case 1: the ball is initially above the bottom wall and is going down
				// case 2: the ball is initially below the top wall and is going up
				// case 3: the case is initially to the left of the right wall and is going right
				// case 4: the ball is initially to the right of the left wall and is going left
				// Add code below
                if (locY >= pt && vY > 0){
                    vY = -vY;
                }
                if(locY <= pb && vY < 0){
                    vY = -vY;
                }
                if(locX >= pr && vX > 0){
                    vX = -vX;
                }
                if(locX <= pl && vX < 0){
                    vX = -vX;
                }



				// add code above
			}
		}
	}
	
	void draw(){
		GLfloat angle;
		glColor3f(r,g,b);// color of ball
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_POLYGON); // The stack to draw the ball
		// add code below to compute and draw points on a circle
		
//        GLfloat locX, locY, radius;// location and radius
        angle = 3.141592653/360;
//        int a = 10; a < 20; a = a + 1
        for (int i=0; i < 720; i++)
        {
            float degInRad = i*angle;
            glVertex2f(cos(degInRad)*radius+locX,sin(degInRad)*radius+locY);
        }

		//add code above to compute and draw points on a circle
		glEnd();
		glFlush();
	}
};
#endif

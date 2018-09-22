#ifndef BLOCK_H
#define BLOCK_H
#include "box.h"
#include <math.h>
class block: public box{
	GLfloat centerX, centerY; // center of the paddle
	GLfloat height, width; // width and height of the paddle
public:
	block(GLfloat cx, GLfloat cy, GLfloat h, GLfloat w, GLfloat red, GLfloat green, GLfloat blue, bool f):box(cy+h/2.0f,cy-h/2.0f, cx-w/2.0f, cx+w/2.0f,red, green,blue, f)
	{
		centerX = cx;
		centerY = cy;
		height = h;
		width = w;
	}
	void setLoc(GLfloat cx, GLfloat cy){
		centerX = cx;
		centerY = cy;
		setBox(cy+height/2.0f,cy-height/2.0f, cx-width/2.0f, cx+width/2.0f);
	};
	void setLoc(GLfloat cx){
		centerX = cx;
		setBox(centerY+height/2.0f,centerY-height/2.0f, cx-width/2.0f, cx+width/2.0f);
	};
};
#endif
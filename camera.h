#include <GL/glut.h>
#include <stdio.h>
#include "extramath.h"


typedef struct
{
	GLposition pos;
	GLposition la;
	GLvector up;
} GLcamera;


void defaultCamera(GLcamera *c);
void processCamera(GLcamera *c);

void moveForward(GLcamera *c, GLfloat v);
void moveBackward(GLcamera *c, GLfloat v);
void moveUp(GLcamera *c, GLfloat v);
void moveDown(GLcamera *c, GLfloat v);
void turnLeft(GLcamera *c, GLfloat phi);
void turnRight(GLcamera *c, GLfloat phi);

void printCamera(GLcamera *c);
void levelHead(GLcamera *c);

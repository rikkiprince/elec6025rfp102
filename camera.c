#include "camera.h"

void defaultCamera(GLcamera *c)
{
	//c->pos.x = 150.0;
	//c->pos.y = 1000.0;
	c->pos.x = 10.0;
	c->pos.y = 10.0;
	c->pos.z = 0.1;
	c->pos.y = 1.80;
	c->pos.z = 5.0;
	//c->pos.y = 50.0;
	//c->pos.z = 50.0;

	c->pos.x = 3.0;
	c->pos.y = 3.0;
	c->pos.z = 15.0;

	c->la.x = 0.0;
	c->la.y = 0.0;
	c->la.z = 0.0;

	c->up.i = 0.0;
	c->up.j = 1.0;
	c->up.k = 0.0;

	//levelHead(c);
}

void processCamera(GLcamera *c)
{
	gluLookAt(	c->pos.x, c->pos.y, c->pos.z, 
				c->la.x,  c->la.y,  c->la.z,
				c->up.i,  c->up.j,  c->up.k);
}


void moveForward(GLcamera *c, GLfloat v)
{
	GLfloat dx = c->la.x - c->pos.x, dy = c->la.y - c->pos.y, dz = c->la.z - c->pos.z;
	GLfloat ratio = v / (sqrt((dx*dx)+(dy*dy)+(dz*dz)));

	dx *= ratio;
	dy *= ratio;
	dz *= ratio;

	c->pos.x += dx;
	c->pos.y += dy;
	c->pos.z += dz;

	c->la.x += dx;
	c->la.y += dy;
	c->la.z += dz;
}

void moveBackward(GLcamera *c, GLfloat v)
{
	moveForward(c, -v);
}

void moveUp(GLcamera *c, GLfloat v)
{
	c->pos.y += v;
	levelHead(c);
}

void moveDown(GLcamera *c, GLfloat v)
{
	moveUp(c, -1.0*v);
}

void turnLeft(GLcamera *c, GLfloat phi)
{
	GLfloat cx = c->la.x - c->pos.x, cz = c->la.z - c->pos.z;

	c->la.x = c->pos.x + (cx * cos(phi)) + (cz * sin(phi));
	c->la.z = c->pos.z + (cz * cos(phi)) - (cx * sin(phi));
}

void turnRight(GLcamera *c, GLfloat phi)
{
	turnLeft(c, -phi);
}

void printCamera(GLcamera *c)
{
	printf("Camera\n======\nPos:\tx: %f, y: %f, z: %f\nLookAt:\tx: %f, y: %f, z: %f\nUp:\tx: %f, y: %f, z: %f\n\n",
		c->pos.x, c->pos.y, c->pos.z, 
		c->la.x, c->la.y, c->la.z, 
		c->up.i, c->up.j, c->up.k);
}

void levelHead(GLcamera *c)
{
	c->la.y = c->pos.y;
}


#ifndef __EXTRAMATH_H__
#define __EXTRAMATH_H__

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>


#define PI				3.14159265
#define DEG2RAD(d)		(d*PI/180.0)
#define RAD2DEG(d)		(d*180.0/PI)
#define M_PI			PI


typedef struct
{
	GLfloat i;
	GLfloat j;
	GLfloat k;
} GLnormal, GLvector;

typedef struct
{
	GLfloat x;
	GLfloat y;
	GLfloat z;

	GLnormal n;
} GLvertex;

typedef struct
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
} GLposition;

/*typedef struct
{
	GLvertex v;
} GLlight;*/

typedef struct
{
	GLfloat r;
	GLfloat g;
	GLfloat b;
} GLrgb;

typedef struct
{
	GLvertex tr;
	GLvertex tl;
	GLvertex bl;
	GLvertex br;

	GLnormal n;
} GLquad;


typedef struct
{
	GLposition p;
	GLrgb a;
	GLrgb d;
	GLrgb s;
} GLlight;

typedef struct
{
	GLrgb a;
	GLrgb d;
	GLrgb s;
	GLfloat f;
} GLmaterial;

void vertexSubtract(GLvector *c, GLvertex b, GLvertex a);
void vectorCrossProduct(GLvector *c, GLvector u, GLvector v);
GLfloat vectorMagnitude(GLvector c);
void vectorNormalizeP(GLvector *c, char *s);
void vectorNormalize(GLvector *c);
GLfloat vectorDotProduct(GLvector a, GLvector b);
GLfloat maxf(GLfloat a, GLfloat b);
GLvector vectorAdd(GLvector a, GLvector b);
GLvector vectorSubtract(GLvector b, GLvector a);
GLvector vectorBetween(GLvertex from, GLvertex to);
GLvector svMultiply(GLfloat s, GLvector v);
GLnormal planeNormal(GLvertex a, GLvertex b, GLvertex c);

#endif

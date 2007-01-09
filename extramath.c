#include "extramath.h"

void vertexSubtract(GLvector *c, GLvertex b, GLvertex a)
{
	c->i = b.x-a.x;
	c->j = b.y-a.y;
	c->k = b.z-a.z;
}

void vectorCrossProduct(GLvector *c, GLvector u, GLvector v)
{
	c->i = ((u.j * v.k) - (u.k * v.j));
	c->j = ((u.k * v.i) - (u.i * v.k));
	c->k = ((u.i * v.j) - (u.j * v.i));
}

GLfloat vectorMagnitude(GLvector c)
{
	return (GLfloat)sqrt((c.i * c.i)+(c.j * c.j)+(c.k * c.k));
}

void vectorNormalizeP(GLvector *c, char *s)
{
	GLfloat mag = vectorMagnitude(*c);

	printf("Normalizing %s: [%f %f %f]\n", s, c->i, c->j, c->k);

	c->i = c->i/mag;
	c->j = c->j/mag;
	c->k = c->k/mag;

	printf("Result %s: [%f %f %f]\n\n", s, c->i, c->j, c->k);
}

void vectorNormalize(GLvector *c)
{
	GLfloat mag = vectorMagnitude(*c);

	c->i = c->i/mag;
	c->j = c->j/mag;
	c->k = c->k/mag;
}

GLfloat vectorDotProduct(GLvector a, GLvector b)
{
	return (a.i*b.i)+(a.j*b.j)+(a.k*b.k);
}

GLfloat maxf(GLfloat a, GLfloat b)
{
	return (a>b)?a:b;
}

/*void vectorAdd(GLvector *c, GLvector a, GLvector b)
{
	c->i = a.i + b.i;
	c->j = a.j + b.j;
	c->k = a.k + b.k;
}*/

GLvector vectorAdd(GLvector a, GLvector b)
{
	GLvector c;
	c.i = a.i + b.i;
	c.j = a.j + b.j;
	c.k = a.k + b.k;

	return c;
}

GLvector vectorSubtract(GLvector b, GLvector a)
{
	GLvector c;
	c.i = b.i - a.i;
	c.j = b.j - a.j;
	c.k = b.k - a.k;

	return c;
}

GLvector vectorBetween(GLvertex from, GLvertex to)
{
	GLvector u;

	u.i = to.x - from.x;
	u.j = to.y - from.y;
	u.k = to.z - from.z;

	return u;
}

GLvector svMultiply(GLfloat s, GLvector v)
{
	GLvector c;
	c.i = v.i * s;
	c.j = v.j * s;
	c.k = v.k * s;

	return c;
}

GLnormal planeNormal(GLvertex a, GLvertex b, GLvertex c)
{
	GLvector A = vectorBetween(a, b);
	GLvector B = vectorBetween(a, c);

	GLnormal N;

	vectorCrossProduct(&N, A, B);
	vectorNormalize(&N);

	return N;
}
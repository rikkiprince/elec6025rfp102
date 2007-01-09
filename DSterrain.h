#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include "extramath.h"


typedef struct
{
	int size;
	float *array;
} DSterrain;

void createDSTerrain(DSterrain *dst, int size);
void destroyDSTerrain(DSterrain *dst);

void diamond(DSterrain *dst, int t, int l, int b, int r, float hr);
void square(DSterrain *dst, int t, int l, int b, int r, float hr);

float getHeight(DSterrain *dst, int x, int y);
void setHeight(DSterrain *dst, int x, int y, float value);

double random();

void printDSTerrain(DSterrain *dst);

void drawDSTerrain(DSterrain *dst);
void drawDSTerrainWireframe(DSterrain *dst);
void drawDSTerrainPolygon(DSterrain *dst);

#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <string.h>
#include <malloc.h>
#include "extramath.h"
#include "textfile.h"


typedef struct
{
	GLuint v;	// vertex shader
	GLuint f;	// fragment shader
	GLuint p;	// shader program
} GLshader;

void createShader(GLshader *s, char *vert, char *frag);
void useShader(GLshader *s);
void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);

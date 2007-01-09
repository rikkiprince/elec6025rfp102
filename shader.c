#include "shader.h"


void createShader(GLshader *s, char *vert, char *frag)
{
	char *vs;
	char *fs;

	// check whether arguments exist
	if(strlen(vert) <= 0 && strlen(frag) <= 0)
	{
		printf("You did not provide any filenames of shader source!\n");
		return;
	}

	vs = textFileRead(vert);
	fs = textFileRead(frag);

	if(vs == NULL && fs == NULL)
	{
		printf("Both shader files had no text!\n");
		return;
	}

	s->p = glCreateProgram();

	if(vs != NULL)
	{
		//char *vs;

		s->v = glCreateShader(GL_VERTEX_SHADER);

		//vs = textFileRead(vert);

		glShaderSource(s->v, 1, &vs, NULL);	// what do these args mean?

		free(vs);

		glCompileShader(s->v);

		glAttachShader(s->p, s->v);
	}

	if(fs != NULL)
	{
		//char *fs = textFileRead(frag);

		s->f = glCreateShader(GL_FRAGMENT_SHADER);

		//fs = textFileRead(frag);

		glShaderSource(s->f, 1, &fs, NULL);

		free(fs);

		glCompileShader(s->f);

		glAttachShader(s->p, s->f);
	}

	// link and use program.
	// maybe not use it?  maybe have other function to use it?  or maybe use by default?
	glLinkProgram(s->p);
	glUseProgram(s->p);
}

void useShader(GLshader *s)
{
	glUseProgram(s->p);
}

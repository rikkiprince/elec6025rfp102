#include "shader.h"


void createShader(GLshader *s, char *vert, char *frag)
{
	char *vs;
	char *fs;

	int param;

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

	glGetShaderiv(s->v, GL_COMPILE_STATUS, &param);
	if(param == GL_FALSE)
	{
		printf("Vertex shader failed to compile.\n");
		printShaderInfoLog(s->v);
	}
	glGetShaderiv(s->f, GL_COMPILE_STATUS, &param);
	if(param == GL_FALSE)
	{
		printf("Fragment shader failed to compile.\n");
		printShaderInfoLog(s->f);
	}
	glGetProgramiv(s->p, GL_LINK_STATUS, &param);
	if(param == GL_FALSE)
	{
		printf("Program failed to link.\n");
		printProgramInfoLog(s->p);
	}
}

void useShader(GLshader *s)
{
	glUseProgram(s->p);
}


// http://www.lighthouse3d.com/opengl/glsl/index.php?oglinfo
void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
	    infoLog = (char *)malloc(infologLength);
	    glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
	    free(infoLog);
	}
}

// http://www.lighthouse3d.com/opengl/glsl/index.php?oglinfo
void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0)
	{
	    infoLog = (char *)malloc(infologLength);
	    glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
	    free(infoLog);
	}
}
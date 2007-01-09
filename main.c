#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include "camera.h"
#include "shader.h"
#include "dsterrain.h"
#include "extramath.h"

#include <time.h>

GLcamera camera;

GLshader toon;
GLshader minimal;
GLshader black;

GLuint fbo;
GLuint depthbuffer;
GLuint img;
GLenum status;
GLuint render_texture;

int rttWidth = 512, rttHeight = 512;

DSterrain terrain;

time_t prevTime;
int prevMillis;
int frameCount = 0;
double idleCount = 0;

int whichToonRenderer = 2;

typedef struct
{
	GLvector pos;
	GLfloat size;
} CloudBlob;

#define NUM_CLOUD_BLOBS (10)

typedef struct
{
	CloudBlob *cloud;
	int arrsize;
	float a, b;
} Cloud;

Cloud cloud;

void drawAxes(GLfloat l);
void drawCloudR(float ss, float sd, int depth, int a, int b);
void renderCloud(Cloud c);

void drawCloud(void)
{
	int a=10, b=10;
	glPushMatrix();

	//useShader(&minimal);
	/*glColor3f( 1.0f, 1.0f, 1.0f );
	glutSolidSphere(1, a, b);
	glPushMatrix();
		glTranslatef(0.5f, 0.5f, 0.5f);
		glutSolidSphere(0.75, a, b);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(-0.5f, 0.5f, 0.5f);
		glutSolidSphere(0.75, a, b);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0.5f, -0.5f, 0.5f);
		glutSolidSphere(0.75, a, b);
	glPopMatrix();*/

	glColor3f( 1.0f, 1.0f, 1.0f );
	//glutSolidSphere(1, a, b);
	//drawCloudR(1.0, 0.1, 2, a, b);
	glTranslatef(1.0f, 1.0f, 1.0f);
	renderCloud(cloud);

	glPopMatrix();
}

void renderCloud(Cloud c)
{
	int blob=0;
	CloudBlob prevBlob = c.cloud[0];

	printf("Rendering cloud...\n");

	glPushMatrix();
	glTranslatef(prevBlob.pos.i, prevBlob.pos.j, prevBlob.pos.k);

	for(blob=0; blob<c.arrsize; blob++)
	{
		printf("blob at: (%f, %f, %f)\n", c.cloud[blob].pos.i, c.cloud[blob].pos.j, c.cloud[blob].pos.k);
		glTranslatef(c.cloud[blob].pos.i - prevBlob.pos.i, c.cloud[blob].pos.j - prevBlob.pos.j, c.cloud[blob].pos.k - prevBlob.pos.k);
		glutSolidSphere(c.cloud[blob].size, c.a, c.b);
		prevBlob = c.cloud[blob];
	}
	glPopMatrix();
}

// returns current index
int createCloudR(Cloud *c, int index, float ss, float sd, float px, float py, float pz, int maxdepth)
{
	float dx = (random() * 20 - 10);
	float dy = (random() * 20 - 10);
	float dz = (random() * 20 - 10);
	float n=1;
	int i=0;

	if(index >= c->arrsize || maxdepth <= 0)
		return index;

	n = sqrt((dx*dx) + (dy*dy) + (dz*dz));

	dx = ss * dx/n;
	dy = ss * dy/n;
	dz = ss * dz/n;

	ss -= sd;

	c->cloud[index].pos.i = px + dx;
	c->cloud[index].pos.j = py + dy;
	c->cloud[index].pos.k = pz + dz;

	c->cloud[index].size = ss;

	index++;
	if(index >= c->arrsize)
		return index;

	for(i=0; i<3; i++)
	{
		index = createCloudR(c, index, ss, sd, px + dx, py + dy, pz + dz, maxdepth-1);
		if(index >= c->arrsize)
			return index;
	}

	return index;
}

void createCloud(Cloud *c, int arrsize, float a, float b, int maxdepth)
{
	c->cloud = malloc(sizeof(CloudBlob) * arrsize);
	memset(c->cloud, 0, sizeof(CloudBlob) * arrsize);

	c->arrsize = arrsize;
	c->a = a;
	c->b = b;

	createCloudR(c, 0, 1.0, 0.1, 0.0f, 0.0f, 0.0f, maxdepth);
}

void destroyCloud(Cloud *c)
{
	free(c->cloud);
}

void drawCloudR(float ss, float sd, int depth, int a, int b)
{
	float dx = (random() * 20 - 10);
	float dy = (random() * 20 - 10);
	float dz = (random() * 20 - 10);
	float n=1;

	if(depth == 0)
		return;

	n = sqrt((dx*dx) + (dy*dy) + (dz*dz));

	dx = dx/n;
	dy = dy/n;
	dz = dz/n;

	ss -= sd;

	glPushMatrix();
		glTranslatef(dx, dy, dz);
		glutSolidSphere(ss, a, b);
		drawCloudR(ss, sd, depth-1, a, b);
	glPopMatrix();
}

void drawClouds(void)
{
	glPushMatrix();
		glTranslatef(0.0, 5.0, 0.0);
		drawCloud();
	glPopMatrix();
}

void drawToonGeometry(void)
{
	glColor3f(1.0f, 0.5f, 0.5f);
	glutSolidTeapot(1);
	drawDSTerrain(&terrain);
	drawClouds();
}

void plainToonShader(void (*geom)(void))
{
	useShader(&toon);
	(*geom)();
}

void lineBasedOutline(void (*geom)(void))
{
	glColor3f(0.0f, 0.0f, 0.0f);
	// http://www.codeproject.com/opengl/dsaqua.asp
	// Push the GL attribute bits so that we don't wreck any settings
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	// Enable polygon offsets, and offset filled polygons forward by 2.5
	glEnable( GL_POLYGON_OFFSET_FILL );
	glPolygonOffset( -2.5f, -2.5f );
	// Set the render mode to be line rendering with a thick line width
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glLineWidth( 5.0f );
	// Set the colour to be white
	glColor3f( 0.0f, 0.0f, 0.0f );
	// Render the object
	useShader(&black);
	(*geom)();
	// Set the polygon mode to be filled triangles 
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable( GL_LIGHTING );
	// Set the colour to the background
	glColor3f( 1.0f, 1.0f, 1.0f );
	// Render the object
	useShader(&toon);
	(*geom)();
	// Pop the state changes off the attribute stack
	// to set things back how they were
	glPopAttrib();
}

void display(void)
{
	// RTT
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0,0,rttWidth, rttHeight);


	// Render as normal here
	// output goes to the FBO and it’s attached buffers

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear frame and z-buffers
	glLoadIdentity();									// load identity matrix

	processCamera(&camera);

	useShader(&minimal);
	glColor3f(114.0/255.0, 181.0/255.0, 245.0/255.0);
	glutSolidSphere(100, 10, 10);

	useShader(&minimal);
	drawAxes(10);

	switch(whichToonRenderer)
	{
		case 1:	lineBasedOutline(&drawToonGeometry); break;
		case 2:	plainToonShader(&drawToonGeometry); break;
		default: drawToonGeometry(); break;
	}/**/

	// end RTT
	glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);


	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear frame and z-buffers
	//glLoadIdentity();									// load identity matrix


	useShader(&minimal);
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, img);
	glScalef(5.0, 5.0, 1.0);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(0.0, 0.0, 1.0);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(1.0, 0.0, 1.0);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(1.0, 1.0, 1.0);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(0.0, 1.0, 1.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);

/*	useShader(&toon);
	glutSolidTeapot(1);
	glLineWidth( 3.0f );
	glutWireTeapot(1);*/


	//glColor3f(1.0, 1.0, 1.0);
	//useShader(&toon);
	//drawDSTerrain(&terrain);
	
	frameCount++;

	glFlush();
	glutSwapBuffers();
}

void special(int k, int x, int y)
{
	switch(k)
	{
		case GLUT_KEY_UP: /*printf("1, ");*/ moveForward(&camera, 1.0); break;//camera_velocity++;	break;	//
		case GLUT_KEY_DOWN: /*printf("2, ");*/ moveBackward(&camera, 1.0); break;//camera_velocity--;	break;	//
		case GLUT_KEY_LEFT: /*printf("3, ");*/ turnLeft(&camera, DEG2RAD(5)); break;
		case GLUT_KEY_RIGHT: /*printf("4, ");*/ turnRight(&camera, DEG2RAD(5)); break;
		case GLUT_KEY_PAGE_UP: /*printf("UP!\n");*/	moveUp(&camera, 1.0); break;
		case GLUT_KEY_PAGE_DOWN: moveDown(&camera, 1.0); break;
	}

	//printPos();

	//printCamera(&camera);

	display();
}

void keyboard(unsigned char k, int x, int y)
{
	if(k>='0' && k<='9')
	{
		whichToonRenderer = k - 48;	// to convert ascii number to actual int value, take off 48
		printf("k = %d, r = %d\n", k, whichToonRenderer);
	}

	switch(k)
	{
		case 'q':
		case 'Q':
		case  27:	printf("End of program...\n");
					exit(0);
					break;
	}

	//printPos();

	display();
}

void initRenderToTexture()
{
	// http://www.gamedev.net/reference/programming/features/fbo1/

	// Setup our FBO
	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

	// Create the render buffer for depth	
	glGenRenderbuffersEXT(1, &depthbuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, rttWidth, rttHeight);

	// Now setup a texture to render to
	glGenTextures(1, &img);
	glBindTexture(GL_TEXTURE_2D, img);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  rttWidth, rttHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//  The following 3 lines enable mipmap filtering and generate the mipmap data so rendering works
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glGenerateMipmapEXT(GL_TEXTURE_2D);

	// And attach it to the FBO so we can render to it
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, img, 0);

	// Attach the depth render buffer to the FBO as it's depth attachment
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthbuffer);
	

	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
		exit(1);
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);	// Unbind the FBO for now
}

void initialisation()
{
	// initialise position of camera.
	defaultCamera(&camera);
	levelHead(&camera);

	// setup shaders
	//createShader(&toon, "toonf2.vert", "toonf2.frag");
	createShader(&toon, "my_toonf2.vert", "my_toonf2.frag");
	//createShader(&minimal, "minimal.vert", "minimal.frag");
	createShader(&black, "minimal.vert", "black.frag");

	createDSTerrain(&terrain, 6);
	//printDSTerrain(&terrain);
	//destroyDSTerrain(&terrain);

	createCloud(&cloud, 30, 10, 10, 6);

	initRenderToTexture();

	prevTime = time(NULL);
}

void deinitialisation()
{
	// destroy shaders
	//destroyShader(&toon);
	//destroyShader(&minimal);

	destroyDSTerrain(&terrain);
	destroyCloud(&cloud);
}

void idle(void)
{
	unsigned int millis;

	idleCount++;
	if(difftime(time(NULL), prevTime) > 1.0)
	{
		//printf("FPS: %d\n", frameCount);
		prevTime = time(NULL);
		frameCount = 0;
	}

	millis = glutGet(GLUT_ELAPSED_TIME);
	if((millis - prevMillis) > 1000)
	{
		//printf("idle count: %d\n", idleCount*1000.0/(millis - prevMillis));
		prevMillis = millis;
		idleCount = 0;
	}
}

void reshape(int w, int h)
{
	//GLfloat nw, nh;

	glViewport(0, 0, w, h);			// set up viewport based on width and height
	glMatrixMode(GL_PROJECTION);	// set projection mode
	glLoadIdentity();				// load identity matrix

	// WTF does the following code do?
	/*if(w<=h)
		glOrtho(-2.0, 2.0, -2.0 * (GLfloat)h / (GLfloat)w, 2.0 * (GLfloat)h / (GLfloat)w, -100.0, 100.0);
	else
		glOrtho(-2.0 * (GLfloat)h / (GLfloat)w, 2.0 * (GLfloat)h / (GLfloat)w, -2.0, 2.0, -100.0, 100.0);*/

	// this is how I think it should work!!
	//nw = w/250.0;
	//nh = h/250.0;
	//printf("w: %i h: %i\nnw: %f nh: %f\n\n", w, h, nw, nh);
	//glOrtho(-nw, nw, -nh, nh, -50.0, 50.0);
	//glFrustum(-15.0, 15.0, -15.0, 15.0, -50.0, 50.0);
	//glFrustum(-nw, nw, -nh, nh, -50.0, 50.0);

	//printf("w: %i, h: %i ratio(w/h): %f, ratio(h/w): %f\n", w, h, (GLfloat)w/(GLfloat)h, (GLfloat)h/(GLfloat)w);
	gluPerspective(45, (GLfloat)w/(GLfloat)h, 1.0, 2000.0);

	glMatrixMode(GL_MODELVIEW);
}

void joystick(unsigned int buttonMask, int x, int y, int z)
{
	printf("JOYSTICK!\n");
}

void main(int argc, char **argv) {
	
	printf("Program begins!");

	// Window and display setup
	glutInit(&argc, argv);			// Have to initialise GLUT - may have args on command line, to change resolution etc - let GLUT do this
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);	// double buffering (for screen 4), RGB for nice colours and Z-buffering
	glutInitWindowSize(500, 500);	// Set window width and height...
	//glutInitWindowSize(1280, 1024);	// Set window width and height...
	glutCreateWindow("~rfp102 - Rikki Prince - ELEC6025 Advanced Graphics Project");	// Create window and give it a title

	// Callback setup
	glutReshapeFunc(reshape);	// handle changing the window size which affects the viewport
	glutDisplayFunc(display);	// important!  draws on screen!

	glutKeyboardFunc(keyboard);	// handles keyboard events
	glutSpecialFunc(special);	// handle special keys up, down, left right, home, F1, F2 etc etc...
	glutIdleFunc(idle);			// what to do when bored - animate?
	//glutMouseFunc(mouse);		// handle mouse events
	glutJoystickFunc(joystick, 10);
	
	//glutMotionFunc(active);
	//glutPassiveMotionFunc(passive);
	//glutTimerFunc(1, timer, 1);// handle timer events

	glEnable(GL_DEPTH_TEST);	// enable hidden surface removal - shouldn't this be somewhere else?
	glShadeModel(GL_SMOOTH);



	
	glewInit();
	if (glewIsSupported("GL_VERSION_2_0"))
		printf("Ready for OpenGL 2.0\n");
	else {
		printf("OpenGL 2.0 not supported\n");
		exit(1);
	}
	/*glewInit();
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
	else {
		printf("Not totally ready :( \n");
		exit(1);
	}*/


	initialisation();


	glutMainLoop();


	deinitialisation();
}


void drawAxes(GLfloat l)
{
	//glLoadIdentity();
	glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);		// red X-axis
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(l, 0.0, 0.0);

		glColor3f(0.0, 1.0, 0.0);		// green Y-axis
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, l, 0.0);


		glColor3f(0.0, 0.0, 1.0);		// blue Z-axis
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, l);

	glEnd();
}

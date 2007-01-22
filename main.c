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
GLshader normalshader;
GLshader depthshader;
GLshader depthnormal;
GLshader discontinuity;
GLshader textureSimple;
GLshader depthoutline;
GLshader normaloutline;
GLshader final;

GLuint fbo;
GLuint depthbuffer;
GLuint img;
GLenum status;
GLuint render_texture;

int rttWidth = 1024, rttHeight = 1024;

int winWidth, winHeight;

DSterrain terrain;

time_t prevTime;
int prevMillis;
int frameCount = 0;
double idleCount = 0;

char *maintitle = "~rfp102 - Rikki Prince - ELEC6025 Advanced Graphics Project";




#define LONGITUDINALS	(36)
#define LATITUDINALS	(36)							// breaks above 72 - possibly due to angle calculation?
#define TOTAL			(LONGITUDINALS * LATITUDINALS)

//GLvertex ind[TOTAL];
//GLnormal norm[TOTAL];
GLquad quads[TOTAL];
int q=0;	// quad count

int generateQuads(GLfloat r, int longs, int lats, GLquad quads[]);
void drawQuads(int q, GLquad quads[]);







void drawToonGeometry(void);
void plainToonShader(void (*geom)(void), GLshader *shader);
void renderWithShader(void (*geom)(void), GLshader *shader);
void lineBasedOutline(void (*geom)(void), GLshader *shader);
void stencilBasedOutline(void (*geom)(void), GLshader *shader);


typedef struct
{
	void (*render)(void (*geom)(void), GLshader *shader);
	GLshader *preshader;
	GLshader *postshader;
} Renderer;

#define MAX_RENDERERS	(9)
int whichToonRenderer = 2;

Renderer renderer[MAX_RENDERERS] = {	{&plainToonShader,	&toon,			&minimal},
										{&lineBasedOutline,	&minimal,		&minimal},
										{&renderWithShader,	&depthshader,	&minimal},
										{&renderWithShader,	&normalshader,	&minimal},
										{&renderWithShader,	&depthnormal,	&depthoutline},
										{&renderWithShader,	&depthnormal,	&normaloutline},
										{&renderWithShader,	&depthnormal,	&final},
										{&renderWithShader,	&depthnormal,	&textureSimple},
										{&stencilBasedOutline,	&minimal,	&minimal},
									};

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

	//printf("Rendering cloud...\n");

	glPushMatrix();
	glTranslatef(prevBlob.pos.i, prevBlob.pos.j, prevBlob.pos.k);

	for(blob=0; blob<c.arrsize; blob++)
	{
		//printf("blob at: (%f, %f, %f)\n", c.cloud[blob].pos.i, c.cloud[blob].pos.j, c.cloud[blob].pos.k);
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
	// sky dome
	glColor3f(114.0/255.0, 181.0/255.0, 245.0/255.0);
	//glutSolidSphere(100, 10, 10);
	drawQuads(q, quads);

	// set groud colour
	glColor3f(1.0f, 0.5f, 0.5f);
	glutSolidTeapot(1);
	drawDSTerrain(&terrain);

	drawClouds();
	//glutSolidCube(1.0f);
}

void plainToonShader(void (*geom)(void), GLshader *shader)
{
	useShader(&toon);
	(*geom)();
}

void renderWithShader(void (*geom)(void), GLshader *shader)
{
	useShader(shader);
	(*geom)();
}

void lineBasedOutline(void (*geom)(void), GLshader *shader)
{
	glColor3f(0.0f, 0.0f, 0.0f);
	// http://www.codeproject.com/opengl/dsaqua.asp			Why?
	// http://www.codeproject.com/opengl/Outline_Mode.asp
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

void stencilBasedOutline(void (*geom)(void), GLshader *shader)
{
	// http://www.codeproject.com/opengl/Outline_Mode.asp
	// Push the GL attribute bits so that we don't wreck any settings
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glEnable( GL_LIGHTING );
	// Set the clear value for the stencil buffer, then clear it
	glClearStencil(0);
	glClear( GL_STENCIL_BUFFER_BIT );
	glEnable( GL_STENCIL_TEST );
	// Set the stencil buffer to write a 1 in every time
	// a pixel is written to the screen
	glStencilFunc( GL_ALWAYS, 1, 0xFFFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	// Render the object in black
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glColor3f( 0.0f, 0.0f, 0.0f );
	(*geom)();
	glDisable( GL_LIGHTING );
	// Set the stencil buffer to only allow writing
	// to the screen when the value of the
	// stencil buffer is not 1
	glStencilFunc( GL_NOTEQUAL, 1, 0xFFFF );
	glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
	// Draw the object with thick lines
	glLineWidth( 3.0f );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glColor3f( 1.0f, 1.0f, 1.0f );
	(*geom)();
	// Pop the state changes off the attribute stack
	// to set things back how they were
	glPopAttrib();
}

// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=36
void ViewOrtho()							// Set Up An Ortho View
{
	glMatrixMode(GL_PROJECTION);					// Select Projection
	glPushMatrix();							// Push The Matrix
	glLoadIdentity();						// Reset The Matrix
	glOrtho( 0, 640 , 480 , 0, -1, 1 );				// Select Ortho Mode (640x480)
	glMatrixMode(GL_MODELVIEW);					// Select Modelview Matrix
	glPushMatrix();							// Push The Matrix
	glLoadIdentity();						// Reset The Matrix
}

void ViewPerspective()							// Set Up A Perspective View
{
	glMatrixMode( GL_PROJECTION );					// Select Projection
	glPopMatrix();							// Pop The Matrix
	glMatrixMode( GL_MODELVIEW );					// Select Modelview
	glPopMatrix();							// Pop The Matrix
}

int nextPowerOf2(int n)
{
	// http://groups.google.co.uk/group/sci.math/browse_thread/thread/3d1f5af967d981c/6a665f4a4c8edb3a?hide_quotes=no#msg_5ea5f7fdcfdace1b
	int i = 0;
	while(pow(2, i) <= n) i++;

	return pow(2, i);
}

void display(void)
{
	float w = winWidth, h = winHeight;
	GLfloat qs = 0.45;

	// RTT
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0,0,rttWidth, rttHeight);


	// Render as normal here
	// output goes to the FBO and it’s attached buffers

	//glClearColor(1.0, 1.0, 1.0, 0.0);		// has no effect
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear frame and z-buffers
	glLoadIdentity();									// load identity matrix

	processCamera(&camera);

	/*useShader(&minimal);
	glColor3f(114.0/255.0, 181.0/255.0, 245.0/255.0);
	glutSolidSphere(100, 10, 10);*/

	useShader(&minimal);
	//drawAxes(10);

/*	switch(whichToonRenderer)
	{
		case 1:	plainToonShader(&drawToonGeometry); break;
		case 2:	lineBasedOutline(&drawToonGeometry); break;
		case 3: renderWithShader(&drawToonGeometry, &depthshader); break;
		case 4: renderWithShader(&drawToonGeometry, &normalshader); break;
		case 5: renderWithShader(&drawToonGeometry, &depthnormal); break;
		default: drawToonGeometry(); break;
	}*/
	if(whichToonRenderer > 0 && whichToonRenderer <= MAX_RENDERERS)
	{
		renderer[whichToonRenderer-1].render(&drawToonGeometry, renderer[whichToonRenderer-1].preshader);
	}
	else
	{
		drawToonGeometry();
	}

	// end RTT
	glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	
	//glMatrixMode(GL_PROJECTION);	// set projection mode
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f);
	gluLookAt(	0.0f, 0.0f, 0.0f, 
				0.0f,  0.0f,  -1.0f,
				0.0f,  1.0,  0.0f);
	//printf("w: %f h: %f\n", w, h);
	//gluOrtho2D(-1.0f, 1.0f, -w/h, w/h);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear frame and z-buffers
	//glLoadIdentity();									// load identity matrix

	/*switch(whichToonRenderer)
	{
		case 5: useShader(&textureSimple); break;
		case 6: useShader(&depthoutline); break;
		default: useShader(&minimal); break;
	}*/
	if(whichToonRenderer > 0 && whichToonRenderer <= MAX_RENDERERS)
	{
		useShader(renderer[whichToonRenderer-1].postshader);
	}
	else
	{
		useShader(&minimal);
	}
	//useShader(&minimal);
	//useShader(&textureSimple);
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, img);
	//glScalef(5.0, 5.0, 1.0);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-qs*w/h, -qs, 1.0);

		glTexCoord2f(1.0, 0.0);
		glVertex3f( qs*w/h, -qs, 1.0);

		glTexCoord2f(1.0, 1.0);
		glVertex3f( qs*w/h,  qs, 1.0);

		glTexCoord2f(0.0, 1.0);
		glVertex3f(-qs*w/h,  qs, 1.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
	//glMatrixMode(GL_MODELVIEW);

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
	printf("Texture #%d\n", img);
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
	int longs=LONGITUDINALS, lats=LATITUDINALS;

	// initialise position of camera.
	defaultCamera(&camera);
	levelHead(&camera);

	// setup shaders
	//createShader(&toon, "toonf2.vert", "toonf2.frag");
	createShader(&toon, "my_toonf2.vert", "my_toonf2.frag");
	//createShader(&minimal, "minimal.vert", "minimal.frag");
	createShader(&black, "minimal.vert", "black.frag");
	createShader(&depthnormal, "depthnormal.vert", "depthnormal.frag");
	createShader(&depthshader, "depthnormal.vert", "depth.frag");
	createShader(&normalshader, "depthnormal.vert", "normal.frag");
	createShader(&discontinuity, "discontinuity.vert", "discontinuity.frag");
	createShader(&textureSimple, "textureSimple.vert", "textureSimple.frag");
	createShader(&depthoutline, "textureSimple.vert", "depthoutline.frag");
	createShader(&normaloutline, "textureSimple.vert", "maybenormaloutline.frag");
	createShader(&final, "textureSimple.vert", "final.frag");

	createDSTerrain(&terrain, 6);
	//printDSTerrain(&terrain);
	//destroyDSTerrain(&terrain);

	// 1. generate and draw quads
	q = generateQuads(100.0, longs, lats, quads);




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

	//glutPostRedisplay();
}

void reshape(int w, int h)
{
	//GLfloat nw, nh;
	winWidth = w;
	winHeight = h;

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
	glutCreateWindow(maintitle);	// Create window and give it a title

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











void drawQuads(int q, GLquad quads[])
{
	GLrgb col = {1.0, 0.0, 0.0};

	int k=0;


	// draw quads
	for(k=0; k<q; k++)
	{
		glBegin(GL_QUADS);
			glVertex3fv((GLfloat *)&quads[k].tr);
			glVertex3fv((GLfloat *)&quads[k].tl);
			glVertex3fv((GLfloat *)&quads[k].bl);
			glVertex3fv((GLfloat *)&quads[k].br);
		glEnd();
	}
}


void setPoints(GLfloat r, GLfloat th, GLfloat ph, GLfloat *x, GLfloat *y, GLfloat *z)
{
	GLfloat H;

	*y = r * sin(DEG2RAD(ph));
	H = r * cos(DEG2RAD(ph));

	*z = H * sin(DEG2RAD(th));
	*x = H * cos(DEG2RAD(th));
}


void setPointsV(GLfloat r, GLfloat th, GLfloat ph, GLvertex *v)
{
	setPoints(r, th, ph, &v->x, &v->y, &v->z);
}

void sphereNormal(GLvertex *v)
{
	GLvector t;
	GLvertex c = {0.0, 0.0, 0.0};
	
	vertexSubtract(&t, *v, c);
	//vertexSubtract(&t, c, *v);	// make them point inwards?
	vectorNormalize(&t);

	v->n = t;
}

// generate quads into quads[][]
// lots of redundant info
int generateQuads(GLfloat r, int longs, int lats, GLquad quads[])
{
	//GLfloat x, y, z, H;	// x, y, z and temp H variables
	GLfloat th=0, ph=0;		// theta and phi angles

	GLfloat dth=0, dph=0;		// delta for theta and phi angles
	int i=0, j=0;				// loop variables

	int q=0;

	dth = 360/longs;
	dph = 360/lats;

	for(j=0, ph=90; j<lats/2+1; j++, ph+=dph)
	{
		for(i=0, th=0; i<longs; i++, th+=dth)
		{
			setPointsV(r, th,		ph,		&quads[q].tr);
			setPointsV(r, th+dth,	ph,		&quads[q].tl);
			setPointsV(r, th+dth,	ph+dph,	&quads[q].bl);
			setPointsV(r, th,		ph+dph,	&quads[q].br);

			//calculateQuadNormal(&quads[q]);

			/*if(i>0 && j>0)
			{
				GLquad m=quads[q-longs-1];
				GLquad n=quads[q-longs];
				GLquad o=quads[q-1];
				GLquad p=quads[q];

				GLnormal norm;

				norm.i = m.n.i + n.n.i + o.n.i + p.n.i;
				norm.j = m.n.j + n.n.j + o.n.j + p.n.j;
				norm.k = m.n.k + n.n.k + o.n.k + p.n.k;

				vectorNormalize(&norm);

				quads[q].tl.n = norm;
			}*/
			sphereNormal(&quads[q].tr);
			sphereNormal(&quads[q].tl);
			sphereNormal(&quads[q].bl);
			sphereNormal(&quads[q].br);

			q++;
		}
	}

	return q;
}

#include "DSterrain.h"
#include <time.h>

double random()
{
	return ((double)rand()/((double)(RAND_MAX)+(double)(1)));
}

void createDSTerrain(DSterrain *dst, int size)
{
	int s;
	int j, y, x;

	const float H = 0.8f;
	float heightRange = 10.0f;

	// seed random number
	//srand((unsigned int)time((time_t *)NULL));
	srand(10);

	dst->size = size;
	s = pow(2, dst->size) + 1;
	dst->array = malloc(sizeof(float) * s * s);
	memset(dst->array, 0, sizeof(float) * s * s);

	// initialise the corner points:
	setHeight(dst,   0,   0, 1.0f);
	setHeight(dst,   0, s-1, 1.0f);
	setHeight(dst, s-1,   0, 1.0f);
	setHeight(dst, s-1, s-1, 1.0f);

	for(j = s-1; j >= 1; j/=2)
	{
		printf("j = %d\n", j);
		for(y = 0; y < s; y+=j)
		{
			for(x = 0; x < s; x+=j)
			{
				diamond(dst, y, x, y+j, x+j, heightRange);
			}
		}
		for(y = 0; y < s; y+=j)
		{
			for(x = 0; x < s; x+=j)
			{
				square(dst, y, x, y+j, x+j, heightRange);
			}
		}

		heightRange *= pow(2, -1*H);
	}

	//printDSTerrain(dst);
}

void diamond(DSterrain *dst, int t, int l, int b, int r, float hr)
{
	// calc midpoint of curr square
	float tlh = getHeight(dst, t, l);
	float trh = getHeight(dst, t, r);
	float blh = getHeight(dst, b, l);
	float brh = getHeight(dst, b, r);

	float average = (tlh+trh+blh+brh)/4.0f;
	float randHeight = (random() * (hr*2)) - hr;

	//printf("diamond stage.  average=%f, random height=%f\n", average, randHeight);

	setHeight(dst, (l+r)/2, (t+b)/2, average + randHeight);

	//printDSTerrain(dst);
}

void square(DSterrain *dst, int t, int l, int b, int r, float hr)
{
	int width = r - l;
	int depth = b - t;
	int mx = (l+r)/2;
	int my = (t+b)/2;
	float mh = getHeight(dst, mx, my);
	float tlh = getHeight(dst, t, l);
	float trh = getHeight(dst, t, r);
	float blh = getHeight(dst, b, l);
	float brh = getHeight(dst, b, r);
	int s = pow(2, dst->size) + 1;
	// calc midpoint of resulting diamonds
	if(t == 0)
	{
		// do top
		float th = getHeight(dst, mx, my-depth);
		float lh = tlh;
		float rh = trh;
		float bh = mh;

		float randHeight = (random() * (hr*2)) - hr;

		setHeight(dst, mx, b, (th+lh+rh+bh)/4 + randHeight);
	}
	if(l == 0)
	{
		// do left
		float th = tlh;
		float lh = getHeight(dst, mx-width, my);
		float rh = mh;
		float bh = blh;

		float randHeight = (random() * (hr*2)) - hr;

		setHeight(dst, mx, b, (th+lh+rh+bh)/4 + randHeight);
	}
	// do bottom
	{
		float th = mh;
		float lh = blh;
		float rh = brh;
		float bh = getHeight(dst, mx, my+depth);

		float randHeight = (random() * (hr*2)) - hr;

		setHeight(dst, mx, b, (th+lh+rh+bh)/4 + randHeight);

		if(b == s-1)
		{
			setHeight(dst, mx, b, (getHeight(dst, mx, b) + getHeight(dst, mx, 0))/2);
			setHeight(dst, mx, 0, (getHeight(dst, mx, b) + getHeight(dst, mx, 0))/2);
		}
	}
	// do right
	{
		float th = trh;
		float lh = mh;
		float rh = getHeight(dst, mx+width,  my);
		float bh = brh;

		float randHeight = (random() * (hr*2)) - hr;

		setHeight(dst, r, my, (th+lh+rh+bh)/4 + randHeight);
		
		if(r == s-1)
		{
			setHeight(dst, r, my, (getHeight(dst, r, my) + getHeight(dst, 0, my))/2);
			setHeight(dst, 0, my, (getHeight(dst, r, my) + getHeight(dst, 0, my))/2);
		}
	}
}

float getHeight(DSterrain *dst, int x, int y)
{
	float *p;
	int s = pow(2, dst->size) + 1;

	// normalise x,y into array co-ords
	while(x < 0) x+=s;
	while(y < 0) y+=s;
	x = x%s;
	y = y%s;

	p = (dst->array + y*s + x);
	return *p;
}

void setHeight(DSterrain *dst, int x, int y, float value)
{
	float *p;
	int s = pow(2, dst->size) + 1;

	// normalise x,y into array co-ords
	while(x < 0) x+=s;
	while(y < 0) y+=s;
	x = x%s;
	y = y%s;

	//dst->array[y%s][x%s] = value;
	p = (dst->array + y*s + x);
	*p = value;
}

void destroyDSTerrain(DSterrain *dst)
{
	free(dst->array);
}

void printDSTerrain(DSterrain *dst)
{
	int y,x;
	int s = pow(2, dst->size) + 1;
	// print out
	for(y=0; y<s; y++)
	{
		for(x=0; x<s; x++)
		{
			printf("%f ", getHeight(dst, x, y));
		}
		printf("\n");
	}
}

void drawDSTerrain(DSterrain *dst)
{
	int s = pow(2, dst->size) + 1;

	//drawDSTerrainWireframe(dst);
	
	drawDSTerrainPolygon(dst);

	/* REWORK THIS SO IT WORKS
	glPushMatrix();
		drawDSTerrainPolygon(dst);

		glPushMatrix();
		glTranslatef(-(s-1), 0.0, -(s-1));
		drawDSTerrainPolygon(dst);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0, 0.0, -(s-1));
		drawDSTerrainPolygon(dst);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-(s-1), 0.0, 0.0);
		drawDSTerrainPolygon(dst);
		glPopMatrix();
	glPopMatrix();*/
}

void drawDSTerrainWireframe(DSterrain *dst)
{
	int y,x;
	int s = pow(2, dst->size) + 1;

	glPushMatrix();

	glScalef(1.0, 1.0, 1.0);

	glBegin(GL_LINES);

		for(y=0; y<s; y++)
		{
			for(x=0; x<s; x++)
			{
				//printf("%f ", getHeight(dst, x, y));
				if(x > 0)
				{
					float sh = getHeight(dst, x-1, y);
					float eh = getHeight(dst, x, y);
					glVertex3f(x-1, sh, y);
					glVertex3f(x, eh, y);
				}
				if(y > 0)
				{
					float sh = getHeight(dst, x, y-1);
					float eh = getHeight(dst, x, y);
					glVertex3f(x, sh, y-1);
					glVertex3f(x, eh, y);
				}
			}
		}
	glEnd();

	glPopMatrix();
}

void drawDSTerrainPolygon(DSterrain *dst)
{
	int y,x;
	int s = pow(2, dst->size) + 1;

	glPushMatrix();

	glScalef(1.0, 1.0, 1.0);

	glBegin(GL_QUADS);

		for(y=1; y<s; y++)
		{
			for(x=1; x<s; x++)
			{
				//printf("%f ", getHeight(dst, x, y));
				/*if(x > 0)
				{
					float sh = getHeight(dst, x-1, y);
					float eh = getHeight(dst, x, y);
					glVertex3f(x-1, sh, y);
					glVertex3f(x, eh, y);
				}
				if(y > 0)
				{
					float sh = getHeight(dst, x, y-1);
					float eh = getHeight(dst, x, y);
					glVertex3f(x, sh, y-1);
					glVertex3f(x, eh, y);
				}*/
				
				float tlh = getHeight(dst, x-1, y-1);
				float trh = getHeight(dst, x,   y-1);
				float brh = getHeight(dst, x,   y  );
				float blh = getHeight(dst, x-1, y  );

				GLvertex tl = {x-1, tlh, y-1, 0.0};
				GLvertex tr = {x,   trh, y-1, 0.0};
				GLvertex br = {x,   brh, y,   0.0};
				GLnormal n = planeNormal(tr, tl, br);

				glNormal3f(n.i, n.j, n.k);
				glVertex3f(x-1, tlh, y-1);
				glVertex3f(x,   trh, y-1);
				glVertex3f(x,   brh, y);
				glVertex3f(x-1, blh, y);
			}
		}
	glEnd();

	glPopMatrix();
}

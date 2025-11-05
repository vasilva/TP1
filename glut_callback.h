#pragma once
#include <GL/glut.h>

/* GLUT callback Handlers */

static int slices = 20;
static int stacks = 5;
static double rotation = 20.0;
static double zoom = -10.0;
static bool wireView = true;

static void display(void)
{
	const double time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	const double angle = time * 90.0;

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Draw a floor and a cone
	// Floor
	glColor3d(0.0, 0.5, 0.0); // Green
	glPushMatrix();
	glTranslated(0.0, 0.0, zoom);
	glRotated(rotation, 1.0, 0.0, 0.0);
	glRotated(angle, 0.0, 1.0, 0.0);
	glScaled(100.0, 0.2, 100.0);
	glutSolidCube(1.0);
	glPopMatrix();

	// Cone
	glColor3d(1.0, 0.0, 0.0); // Red
	glPushMatrix();
	glTranslated(0.0, 0.1, zoom);
	glRotated(rotation - 90.0, 1.0, 0.0, 0.0);
	glRotated(angle, 0.0, 0.0, 1.0);
	glutSolidCone(1.0, 5.0, slices, stacks);
	glPopMatrix();

	// Wireframe view
	if (wireView)
	{
		glColor3d(0.0, 0.0, 0.0); // Black
		// Wire Floor
		glPushMatrix();
		glTranslated(0.0, 0.0, zoom);
		glRotated(rotation, 1.0, 0.0, 0.0);
		glRotated(angle, 0.0, 1.0, 0.0);
		glScaled(100.0, 0.2, 100.0);
		glutWireCube(1.0);
		glPopMatrix();

		// Wire Cone
		glPushMatrix();
		glTranslated(0.0, 0.1, zoom);
		glRotated(rotation - 90.0, 1.0, 0.0, 0.0);
		glRotated(angle, 0.0, 0.0, 1.0);
		glutWireCone(1.0, 5.0, slices, stacks);
		glPopMatrix();
	}
	// Swap buffers for animation
	glutSwapBuffers();
}

static void reshape(int w, int h)
{
	const GLdouble aspect = (GLdouble)w / (GLdouble)h;

	// Set the viewport to the entire window
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	
	// Set the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the perspective projection
	glFrustum(-aspect, aspect, -1.0, 1.0, 1.5, 100.0);
	
	// Return to modelview matrix mode
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

static void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		wireView = !wireView;
		break;
	
	case '+': // Zoom in
		if (zoom < -5.0)
			zoom += 1.0;
		break;
	
	case '-': // Zoom out
		if (zoom > -50.0)
			zoom -= 1.0;
		break;
	
	case 27: // Escape key
		exit(0);
		break;
	
	default:
		break;
	}
	glutPostRedisplay();
}

static void idle(void)
{
	glutPostRedisplay();
}

/* End of GLUT callback Handlers */
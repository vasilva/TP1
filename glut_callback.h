#pragma once

#include <GL/glut.h>
#include <iostream>

#include "Tower.h"
#include "Floor.h"
#include "Camera.h"
#include "vecFunctions.h"

static Camera* s_camera = nullptr;
static int s_lastX = 0, s_lastY = 0;
static bool s_dragging = false;

/* GLUT callback Handlers */

static void display(void)
{
	const auto time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	s_camera->applyView();

	// Draw a floor and a cone
	// Floor
	Floor floor;
	floor.setPosition(Zero);
	floor.setSize(200.0, 0.5, 200.0);
	floor.setRotation(UnitX * 20.0);
	floor.draw();

	// Cone (Tower)
	Tower tower;
	tower.setPosition(UnitY * 0.1);
	tower.setSize(5.0, 40.0, 5.0);
	tower.setRotation(UnitX * -70.0);
	tower.draw();

	// Camera target visualization
	auto color = Color::Yellow;
	glColor3d(color.x, color.y, color.z); // Yellow color
	auto target = s_camera->getTarget();
	glPushMatrix();
	glTranslated(target.x, target.y, target.z);
	glutSolidSphere(0.2, 20, 20);
	glPopMatrix();

	// Swap buffers for animation
	glutSwapBuffers();
}

static void reshape(int w, int h)
{
	const auto aspect = (double)w / (double)h;

	// Set the viewport to the entire window
	glViewport(0, 0, w, h);

	// Set the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the perspective projection
	glFrustum(-aspect, aspect, -1.0, 1.0, 1.5, 200.0);

	// Return to modelview matrix mode
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

static void cameraKeyboard(unsigned char key, int x, int y)
{
	// Camera movement controls
	// WASD for forward, backward, left, right
	// QE for up and down
	// ZX for zoom in and out
	if (!s_camera) return;
	const double moveAmount = 0.5;
	const double zoomAmount = 1.0;
	switch (key)
	{
	case 'w': case 'W':
		s_camera->moveForward(moveAmount);
		break;
	case 's': case 'S':
		s_camera->moveBackward(moveAmount);
		break;
	case 'a': case 'A':
		s_camera->strafeLeft(moveAmount);
		break;
	case 'd': case 'D':
		s_camera->strafeRight(moveAmount);
		break;
	case 'q': case 'Q':
		s_camera->moveUp(moveAmount);
		break;
	case 'e': case 'E':
		s_camera->moveDown(moveAmount);
		break;
	case 'z': case 'Z':
		s_camera->zoomIn(zoomAmount);
		break;
	case 'x': case 'X':
		s_camera->zoomOut(zoomAmount);
		break;
	case 27: // Escape key
		exit(0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

static void cameraSpecial(int key, int x, int y)
{
	// Camera rotation controls
	// Arrow keys for yaw and pitch
	if (!s_camera) return;
	const double rotateAmount = 5.0;
	switch (key)
	{
	case GLUT_KEY_LEFT:
		s_camera->rotate(-rotateAmount, 0.0);
		break;
	case GLUT_KEY_RIGHT:
		s_camera->rotate(rotateAmount, 0.0);
		break;
	case GLUT_KEY_UP:
		s_camera->rotate(0.0, rotateAmount);
		break;
	case GLUT_KEY_DOWN:
		s_camera->rotate(0.0, -rotateAmount);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

static void cameraMouse(int button, int state, int x, int y)
{
	if (!s_camera) return;

	// Scroll wheel for zooming
	if (state == GLUT_DOWN && button == 3) // Scroll up
	{
		s_camera->zoomIn(1.0);
		glutPostRedisplay();
		return;
	}
	else if (state == GLUT_DOWN && button == 4) // Scroll down
	{
		s_camera->zoomOut(1.0);
		glutPostRedisplay();
		return;
	}

	// Left button for dragging
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			s_dragging = true;
			s_lastX = x;
			s_lastY = y;
		}
		else
		{
			s_dragging = false;
		}
	}
	glutPostRedisplay();
}

static void cameraMotion(int x, int y)
{
	if (!s_camera || !s_dragging) return;

	// Calculate mouse movement delta
	int deltaX = x - s_lastX;
	int deltaY = y - s_lastY;

	// Update last positions
	s_lastX = x;
	s_lastY = y;

	// Adjust camera yaw and pitch based on mouse movement
	const double sensitivity = 0.2;
	s_camera->rotate(deltaX * sensitivity, -deltaY * sensitivity);
	glutPostRedisplay();
}

void registerCameraCallbacks(Camera& cam)
{
	s_camera = &cam;
	glutKeyboardFunc(cameraKeyboard);
	glutSpecialFunc(cameraSpecial);
	glutMouseFunc(cameraMouse);
	glutMotionFunc(cameraMotion);
}

static void idle(void) { glutPostRedisplay(); }

/* End of GLUT callback Handlers */
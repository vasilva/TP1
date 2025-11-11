#pragma once

#include <GL/glut.h>
#include <algorithm>
#include <vector>

#include "Tower.h"
#include "Floor.h"
#include "Flock.h"
#include "Obstacle.h"
#include "ControlledBoid.h"
#include "Camera.h"
#include "World.h"
#include "vecFunctions.h"
#include "HUD.h"

/* GLUT callback Handlers variables */

// Cameras
static Camera* sFollowCamera = nullptr;
static Camera* sFixedCamera = nullptr;
static Camera* sSideCamera = nullptr;

// Camera parameters
static const GLdouble CAMERA_MIN_DISTANCE = 5.0;
static const GLdouble CAMERA_MAX_DISTANCE = 150.0;
static const GLdouble CAMERA_SMOOTH_SPEED = 6.0;
static const GLdouble CAMERA_ZOOM_STEP = 1.5;
static GLdouble sCameraDistance = 40.0;

// Camera types
enum CameraType { FOLLOW_CAMERA = 1, FIXED_CAMERA, SIDE_CAMERA };
static CameraType sCurrentCamera = FOLLOW_CAMERA;

// Scene objects
static Flock* sFlock = nullptr;
static Floor* sFloor = nullptr;
static Tower* sTower = nullptr;
static ControlledBoid* sControlledBoid = nullptr;
static std::vector<Obstacle>* sWalls = nullptr;

// Time tracking
static GLdouble sLastTime = 0.0;
static bool sFullscreen = true;

/* GLUT callback Handlers */

// Display callback: render the scene
static void display(void)
{
	// Calculate delta time
	const GLdouble time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	GLdouble dt;
	if (sLastTime <= 0.0)
		dt = 1.0 / 60.0;
	else
		dt = time - sLastTime;

	sLastTime = time;

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update boids
	if (sControlledBoid) sControlledBoid->update(dt);
	if (sFlock) sFlock->update(dt);

	// Get positions and sizes
	Vec3 cbPos, towerPos, towerSize;
	if (sControlledBoid)
		cbPos = sControlledBoid->getPosition();
	if (sTower)
	{
		towerPos = sTower->getPosition();
		towerSize = sTower->getSize();
	}
	Vec3 flockCenter = sFlock ? sFlock->getAvgPosition() : cbPos;
	Vec3 desiredPos, desiredTarget, currPos, currTarget, smoothPos, smoothTarget;
	Vec3 offset, forwardDir, rightCamPos, right;
	GLdouble yawRad = 0.0, height, t;

	// Camera behavior based on current camera type
	switch (sCurrentCamera)
	{
	case FOLLOW_CAMERA: // Camera follow controlled boid
		if (sFollowCamera && sControlledBoid)
		{
			// Desired camera position and target
			yawRad = sControlledBoid->getYaw() * (PI / 180.0);
			offset = {
				-sin(yawRad) * sCameraDistance,
				sCameraDistance * 0.2,
				-cos(yawRad) * sCameraDistance
			};
			desiredPos = cbPos + offset;
			desiredTarget = cbPos;

			// Current camera position and target
			currPos = sFollowCamera->getPosition();
			currTarget = sFollowCamera->getTarget();

			// Smoothing factor
			t = 1.0 - std::exp(-CAMERA_SMOOTH_SPEED * dt);
			if (t < 0.0) t = 0.0;
			if (t > 1.0) t = 1.0;

			// Interpolate position and target
			smoothPos = lerp(currPos, desiredPos, t);
			smoothTarget = lerp(currTarget, desiredTarget, t);

			// Apply smoothed camera
			sFollowCamera->setPosition(smoothPos);
			sFollowCamera->setTarget(smoothTarget);
			sFollowCamera->applyView();
		}
		break;

	case FIXED_CAMERA: // Camera fixed at the top of the tower
		if (sFixedCamera)
		{
			// Fixed Camera position and target
			height = towerSize.y + sCameraDistance * 0.5;
			sFixedCamera->setPosition(Vec3(towerPos.x, height, towerPos.z));
			sFixedCamera->setTarget(flockCenter);
			sFixedCamera->applyView();
		}
		break;

	case SIDE_CAMERA: // Camera at right of the controlled boid
		if (sControlledBoid && sSideCamera)
		{
			// Side Camera position and target
			yawRad = sControlledBoid->getYaw() * (PI / 180.0);
			forwardDir = { std::sin(yawRad), 0.0, std::cos(yawRad) };
			right = crossProduct(forwardDir, UnitY);
			normalize(right);
			desiredPos = cbPos + right * sCameraDistance + UnitY * sCameraDistance / 5.0;
			desiredTarget = cbPos - forwardDir;

			// Current camera position and target
			currPos = sSideCamera->getPosition();
			currTarget = sSideCamera->getTarget();

			// Smoothing factor
			t = 1.0 - std::exp(-CAMERA_SMOOTH_SPEED * dt);
			if (t < 0.0) t = 0.0;
			if (t > 1.0) t = 1.0;

			// Interpolate position and target
			smoothPos = lerp(currPos, desiredPos, t);
			smoothTarget = lerp(currTarget, desiredTarget, t);

			// Apply smmothed camera
			sSideCamera->setPosition(smoothPos);
			sSideCamera->setTarget(smoothTarget);
			sSideCamera->applyView();
		}
		break;

	default: break;
	}

	// Draw scene objects
	if (sFloor) sFloor->draw();
	if (sTower) sTower->draw();
	if (sControlledBoid) sControlledBoid->draw();
	if (sFlock) sFlock->draw();
	if (sWalls)
		for (auto& w : *sWalls)
			w.draw();

	// Draw HUD overlay
	drawHUD(sFlock ? sFlock->getBoidCount() : 0);

	// Swap buffers for animation
	glutSwapBuffers();
}

// Reshape callback: adjust viewport and projection matrix
static void reshape(int w, int h)
{
	const auto aspect = (double)w / (double)h;

	// Set the viewport to the entire window
	glViewport(0, 0, w, h);

	// Set the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the perspective projection
	glFrustum(-aspect, aspect, -1.0, 1.0, 1.5, 1000.0);

	// Return to modelview matrix mode
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Keyboard callback: handle WASDQE keys for controlled boid
static void keyboardControl(unsigned char key, int x, int y)
{
	const double rotateAmount = 5.0;    // degrees per key press
	const double heightStep = 0.5;      // height change per key press
	if (!sControlledBoid) return;

	switch (key)
	{
		// Movement controls
	case 'w': case 'W': // Accelerate forward
		sControlledBoid->moveForward(1.0);
		break;
	case 's': case 'S': // Decelerate / move backward
		sControlledBoid->moveBackward(1.0);
		break;
	case 'a': case 'A': // Turn left
		sControlledBoid->rotateYaw(rotateAmount);
		break;
	case 'd': case 'D': // Turn right
		sControlledBoid->rotateYaw(-rotateAmount);
		break;
	case 'q': case 'Q': // Increase height
		sControlledBoid->setHeight(sControlledBoid->getHeight() + heightStep);
		{
			auto p = sControlledBoid->getPosition();
			p.y = sControlledBoid->getHeight();
			sControlledBoid->setPosition(p);
		}
		break;
	case 'e': case 'E': // Decrease height
		sControlledBoid->setHeight(sControlledBoid->getHeight() - heightStep);
		{
			auto p = sControlledBoid->getPosition();
			p.y = sControlledBoid->getHeight();
			sControlledBoid->setPosition(p);
		}
		break;

	case 'z': case 'Z': // Stop movement
		sControlledBoid->stop();
		break;

	case 'f': case 'F': // Toggle fullscreen
		if (sFullscreen)
		{
			glutPositionWindow(0, 0);
			glutReshapeWindow(1280, 720);
			sFullscreen = false;
		}
		else
		{
			glutFullScreen();
			sFullscreen = true;
		}

		// Camera switching
	case '1': sCurrentCamera = FOLLOW_CAMERA; break;
	case '2': sCurrentCamera = FIXED_CAMERA; break;
	case '3': sCurrentCamera = SIDE_CAMERA; break;

		// Increase/decrease boid count
	case '+': case '=':
		if (sFlock) sFlock->addBoid();
		break;
	case '-': case '_':
		if (sFlock) sFlock->removeBoid();
		break;

		// Exit
	case 27: exit(EXIT_SUCCESS); break;

	default: break;
	}
	glutPostRedisplay();
}

// Special keys callback: handle arrow keys for controlled boid
static void cameraSpecial(int key, int x, int y)
{
	const double rotateAmount = 5.0; // degrees per key press
	if (!sControlledBoid) return;

	switch (key)
	{
	case GLUT_KEY_LEFT:		// Turn left
		sControlledBoid->rotateYaw(rotateAmount);
		break;
	case GLUT_KEY_RIGHT:	// Turn right
		sControlledBoid->rotateYaw(-rotateAmount);
		break;
	case GLUT_KEY_UP:		// Accelerate forward
		sControlledBoid->moveForward(1.0);
		break;
	case GLUT_KEY_DOWN:		// Decelerate / move backward
		sControlledBoid->moveBackward(1.0);
		break;

	default: break;
	}
	glutPostRedisplay();
}

// Mouse callback: handle zoom with mouse wheel
static void mouseFunc(int button, int state, int x, int y)
{
	// Only react to wheel events when button pressed (most GLUT implementations)
	if (state != GLUT_DOWN) return;

	switch (button)
	{
	case 3: // wheel up -> zoom in (decrease distance)
		sCameraDistance = std::max(CAMERA_MIN_DISTANCE, sCameraDistance - CAMERA_ZOOM_STEP);
		glutPostRedisplay();
		break;

	case 4: // wheel down -> zoom out (increase distance)
		sCameraDistance = std::min(CAMERA_MAX_DISTANCE, sCameraDistance + CAMERA_ZOOM_STEP);
		glutPostRedisplay();
		break;

	default: break;
	}
}

// Idle callback: simply request redisplay
static void idle(void) { glutPostRedisplay(); }

// Function to register world objects for GLUT callbacks
void registerWorldObjects(
	Camera& followCamera, Camera& fixedCamera, Camera& sideCamera,
	Flock& flock, ControlledBoid& controlledBoid,
	Floor& floor, Tower& tower,
	std::vector<Obstacle>& walls)
{
	sFollowCamera = &followCamera;
	sFixedCamera = &fixedCamera;
	sSideCamera = &sideCamera;
	sFlock = &flock;
	sControlledBoid = &controlledBoid;
	sFloor = &floor;
	sTower = &tower;
	sWalls = &walls;
}

/* End of GLUT callback Handlers */
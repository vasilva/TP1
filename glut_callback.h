#pragma once

#include <GL/glut.h>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>

#include "Tower.h"
#include "Floor.h"
#include "Flock.h"
#include "Obstacle.h"
#include "ControlledBoid.h"
#include "Camera.h"
#include "World.h"
#include "vecFunctions.h"
#include "HUD.h"
#include "ObstacleManager.h"

/* GLUT callback Handlers variables */

// Cameras
static Camera* sFollowCamera = nullptr;
static Camera* sFixedCamera = nullptr;
static Camera* sSideCamera = nullptr;

// Camera parameters
static const GLfloat CAMERA_MIN_DISTANCE = 5.0f;
static const GLfloat CAMERA_MAX_DISTANCE = 150.0f;
static const GLfloat CAMERA_SMOOTH_SPEED = 6.0f;
static const GLfloat CAMERA_ZOOM_STEP = 1.5f;
static GLfloat sCameraDistance = 40.0f;

// Camera types
enum CameraType { FOLLOW_CAMERA = 1, FIXED_CAMERA, SIDE_CAMERA };
static CameraType sCurrentCamera = FOLLOW_CAMERA;

// Scene objects
static Flock* sFlock = nullptr;
static Floor* sFloor = nullptr;
static Tower* sTower = nullptr;
static ControlledBoid* sControlledBoid = nullptr;
static std::vector<Obstacle>* sWalls = nullptr;
static ObstacleManager* sObstacleManager = nullptr;

// Time tracking
static GLfloat sLastTime = 0.0f;
static bool sFullscreen = true;
static bool sPaused = false;
static std::vector<std::string> sHUDLines = prepareHUDLines();

// Fog control
static bool sFogEnabled = true;
const Vec3 colorFog = Color::Cyan;
static const GLfloat sFogColor[4] = { colorFog.x, colorFog.y, colorFog.z, 1.0f };
static GLfloat sFogDensity = 0.005f;

/* GLUT callback Handlers */

void enableFog()
{
	// Enable fog and set parameters
	glEnable(GL_FOG);
	glFogfv(GL_FOG_COLOR, sFogColor);
	glFogi(GL_FOG_MODE, GL_EXP);

	// Adjust fog density based on camera distance
	const GLfloat refernceDistance = 40.0f;
	GLfloat scale = sCameraDistance / refernceDistance;
	scale = std::clamp(scale, 0.4f, 3.0f);
	GLfloat density = sFogDensity * (scale * scale);

	// Clamp density to reasonable range
	const GLfloat minDensity = 0.0005f;
	const GLfloat maxDensity = 0.1f;
	if (density < minDensity) density = minDensity;
	if (density > maxDensity) density = maxDensity;

	glFogf(GL_FOG_DENSITY, density);
	
	// Set fog start and end distances
	GLfloat fogStart = std::max(5.0f, sCameraDistance * 0.15f);
	GLfloat fogEnd = std::max(30.0f, sCameraDistance * 2.5f);
	glFogf(GL_FOG_START, fogStart);
	glFogf(GL_FOG_END, fogEnd);
	glHint(GL_FOG_HINT, GL_NICEST);
}

inline void disableFog() { glDisable(GL_FOG); }

// Display callback: render the scene
static void display(void)
{
	// Calculate delta time
	const GLfloat time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	GLfloat dt;
	if (sLastTime <= 0.0f)
		dt = 1.0f / 60.0f;
	else
		dt = time - sLastTime;

	sLastTime = time;

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable or disable fog
	sFogEnabled ? enableFog() : disableFog();

	// Update boids if not paused
	if (!sPaused && sControlledBoid) sControlledBoid->update(dt);
	if (!sPaused && sFlock) sFlock->update(dt);

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
	GLfloat yawRad = 0.0f, height, t;

	// Camera behavior based on current camera type
	switch (sCurrentCamera)
	{
	case FOLLOW_CAMERA: // Camera follow controlled boid
		if (sFollowCamera && sControlledBoid)
		{
			// Desired camera position and target
			yawRad = sControlledBoid->getYaw() * (PI / 180.0f);
			offset = {
				-sin(yawRad) * sCameraDistance,
				sCameraDistance * 0.2f,
				-cos(yawRad) * sCameraDistance
			};
			desiredPos = cbPos + offset;
			desiredTarget = cbPos;

			// Current camera position and target
			currPos = sFollowCamera->getPosition();
			currTarget = sFollowCamera->getTarget();

			// Smoothing factor
			t = 1.0f - std::exp(-CAMERA_SMOOTH_SPEED * dt);
			if (t < 0.0f) t = 0.0f;
			if (t > 1.0f) t = 1.0f;

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
			height = towerSize.y + sCameraDistance * 0.5f;
			sFixedCamera->setPosition(towerPos.x, height, towerPos.z);
			sFixedCamera->setTarget(flockCenter);
			sFixedCamera->applyView();
		}
		break;

	case SIDE_CAMERA: // Camera at right of the controlled boid
		if (sControlledBoid && sSideCamera)
		{
			// Side Camera position and target
			yawRad = sControlledBoid->getYaw() * (PI / 180.0f);
			forwardDir = { std::sin(yawRad), 0.0f, std::cos(yawRad) };
			right = crossProduct(forwardDir, UnitY);
			normalize(right);
			desiredPos = cbPos + right * sCameraDistance + UnitY * sCameraDistance / 5.0f;
			desiredTarget = cbPos - forwardDir;

			// Current camera position and target
			currPos = sSideCamera->getPosition();
			currTarget = sSideCamera->getTarget();

			// Smoothing factor
			t = 1.0f - std::exp(-CAMERA_SMOOTH_SPEED * dt);
			if (t < 0.0f) t = 0.0f;
			if (t > 1.0f) t = 1.0f;

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

	// Disable fog before drawing HUD
	disableFog();

	// Draw HUD overlay
	int boidCount = sFlock ? sFlock->getBoidCount() : 0;
	int obstacleCount = sObstacleManager ? sObstacleManager->size() : 0;
	drawHUD(boidCount, obstacleCount, sHUDLines);

	// Render paused text if simulation is paused
	if (sPaused) drawPausedText();

	// Re-enable fog if it was enabled
	if (sFogEnabled) enableFog();

	// Swap buffers for animation
	glutSwapBuffers();
}

// Reshape callback: adjust viewport and projection matrix
static void reshape(int w, int h)
{
	// Aspect ratio
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
	const GLfloat rotateAmount = 5.0f;	// degrees per key press
	const GLfloat heightStep = 0.5f;	// height change per key press
	const GLfloat minHeight = 2.0f;		// minimum height
	const GLfloat maxHeight = 50.0f;	// maximum height
	if (!sControlledBoid) return;		// No controlled boid available

	switch (key)
	{
		// Movement controls
	case 'w': case 'W': // Accelerate forward
		sControlledBoid->moveForward(1.0f); break;
	case 's': case 'S': // Decelerate / move backward
		sControlledBoid->moveBackward(1.0f); break;
	case 'a': case 'A': // Turn left
		sControlledBoid->rotateYaw(rotateAmount); break;
	case 'd': case 'D': // Turn right
		sControlledBoid->rotateYaw(-rotateAmount); break;

	case 'q': case 'Q': // Increase height
	{
		const GLfloat current = sControlledBoid->getHeight();
		const GLfloat next = std::min(maxHeight, current + heightStep);
		sControlledBoid->setHeight(next);
	}
	break;

	case 'e': case 'E': // Decrease height
	{
		const GLfloat current = sControlledBoid->getHeight();
		const GLfloat next = std::max(minHeight, current - heightStep);
		sControlledBoid->setHeight(next);
	}
	break;

	case ' ': // Pause/unpause simulation
		sPaused = !sPaused;
		break;

	case 'z': case 'Z': // Stop movement
		sControlledBoid->stop();
		break;

	case 'f': case 'F': // Toggle fullscreen
		sFullscreen = !sFullscreen;
		if (sFullscreen) glutFullScreen();
		else
		{
			glutPositionWindow(0, 0);
			glutReshapeWindow(1920, 1080);
		}
		break;

	case 'n': case 'N': // Toggle fog
		sFogEnabled = !sFogEnabled;
		sFogEnabled ? enableFog() : disableFog();
		break;

		// Obstacle management
	case 'o': case 'O': // Add obstacle
		if (sObstacleManager) sObstacleManager->addObstacle();
		break;

	case 'p': case 'P': // Remove obstacle
		if (sObstacleManager) sObstacleManager->removeObstacle();
		break;

	case 'r': case 'R': // Reset obstacles
		if (sObstacleManager && sFloor) sObstacleManager->reset();
		break;

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
	case 27: exit(0); break;

	default: break;
	}
	glutPostRedisplay();
}

// Special keys callback: handle arrow keys for controlled boid
static void cameraSpecial(int key, int x, int y)
{
	const GLfloat rotateAmount = 5.0f; // degrees per key press
	if (!sControlledBoid) return;

	switch (key)
	{
	case GLUT_KEY_LEFT:		// Turn left
		sControlledBoid->rotateYaw(rotateAmount); break;
	case GLUT_KEY_RIGHT:	// Turn right
		sControlledBoid->rotateYaw(-rotateAmount); break;
	case GLUT_KEY_UP:		// Accelerate forward
		sControlledBoid->moveForward(1.0f); break;
	case GLUT_KEY_DOWN:		// Decelerate / move backward
		sControlledBoid->moveBackward(1.0f); break;

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
static inline void idle(void) { glutPostRedisplay(); }

// Function to register world objects for GLUT callbacks
void registerWorldObjects(
	Camera& followCamera, Camera& fixedCamera, Camera& sideCamera,
	Flock& flock, ControlledBoid& controlledBoid,
	Floor& floor, Tower& tower)
{
	sFollowCamera = &followCamera;
	sFixedCamera = &fixedCamera;
	sSideCamera = &sideCamera;
	sFlock = &flock;
	sControlledBoid = &controlledBoid;
	sFloor = &floor;
	sTower = &tower;
}

static void registerObstacleManager(ObstacleManager& mgr)
{
	sObstacleManager = &mgr;
	sWalls = &mgr.getObstacles();
	gWorldObstacles = sWalls;
}

/* End of GLUT callback Handlers */
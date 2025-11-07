#pragma once
#include <GL/glut.h>
#include "Tower.h"
#include "Floor.h"
#include "Flock.h"
#include "ControlledBoid.h"
#include "Camera.h"
#include "vecFunctions.h"

// Cameras
static Camera* sFollowCamera = nullptr;
static Camera* sFixedCamera = nullptr;
static Camera* sSideCamera = nullptr;
static GLdouble sCameraDistance = 15.0;

// Zoom limits / step
static const GLdouble CAMERA_MIN_DISTANCE = 5.0;
static const GLdouble CAMERA_MAX_DISTANCE = 100.0;
static const GLdouble CAMERA_ZOOM_STEP = 1.5;

// Current camera type
enum CameraType { FOLLOW_CAMERA = 1, FIXED_CAMERA, SIDE_CAMERA };
static CameraType sCurrentCamera = FOLLOW_CAMERA;

// Scene objects
static ControlledBoid* sControlledBoid = nullptr;
static Flock* sFlock = nullptr;
static Floor* sFloor = nullptr;
static Tower* sTower = nullptr;

// Time tracking
static GLdouble sLastTime = 0.0;

/* GLUT callback Handlers */

static void display(void)
{
	// Compute time
	const GLdouble time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	GLdouble dt;
	if (sLastTime <= 0.0)
		dt = 1.0 / 60.0;
	else
		dt = time - sLastTime;

	sLastTime = time;

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Controlled boid
	if (!sControlledBoid)
	{
		// allocate persistent objects outside; but header uses static pointers — construct once
		static ControlledBoid controlledBoidInstance;
		sControlledBoid = &controlledBoidInstance;
		sControlledBoid->setPosition(Vec3(20.0, 10.0, 0.0));
	}

	// Floor
	if (!sFloor)
	{
		static Floor floorInstance;
		sFloor = &floorInstance;
		sFloor->setPosition(Zero);
		sFloor->setSize(200.0, 1.0, 200.0);
		sFloor->setRotation(UnitX);
	}

	// Tower
	if (!sTower)
	{
		static Tower towerInstance;
		sTower = &towerInstance;
		sTower->setPosition(UnitY * 0.1);
		sTower->setSize(5.0, 40.0, 5.0);
		sTower->setRotation(UnitX * -90.0);
	}

	// Flock of boids
	if (!sFlock)
	{
		static Flock flockInstance;
		sFlock = &flockInstance;
		sFlock->init(50, 60.0);
	}

	// Update boids
	sControlledBoid->update(dt);
	sFlock->update(dt);

	Vec3 cbPos = sControlledBoid->getPosition();
	Vec3 followCamPos, offset, forwardDir, rightCamPos, right;
	GLdouble yawRad, height;
	switch (sCurrentCamera)
	{
	case FOLLOW_CAMERA: // Camera follow controlled boid
		yawRad = sControlledBoid->getYaw() * (PI / 180.0);
		offset = {
			-sin(yawRad) * sCameraDistance,
			sCameraDistance / 5.0,
			-cos(yawRad) * sCameraDistance
		};
		followCamPos = cbPos + offset;
		if (sFollowCamera)
		{
			sFollowCamera->setPosition(followCamPos);
			sFollowCamera->setTarget(cbPos);
			sFollowCamera->applyView();
		}
		break;

	case FIXED_CAMERA: // Camera fixed at the top of the tower
		if (sTower && sFixedCamera)
		{
			height = sTower->getSize().y + sCameraDistance;
			sFixedCamera->setPosition(Vec3(sTower->getPosition().x, height, sTower->getPosition().z));
			sFixedCamera->setTarget(cbPos);
			sFixedCamera->applyView();
		}
		break;

	case SIDE_CAMERA: // Camera at right of the controlled boid
		yawRad = sControlledBoid->getYaw() * (PI / 180.0);
		forwardDir = { std::sin(yawRad), 0.0, std::cos(yawRad) };
		right = crossProduct(UnitY, forwardDir);
		normalize(right);
		rightCamPos = cbPos + right * sCameraDistance + UnitY * sCameraDistance / 5.0;
		if (sSideCamera)
		{
			sSideCamera->setPosition(rightCamPos);
			sSideCamera->setTarget(cbPos);
			sSideCamera->applyView();
		}
		break;

	default:
		break;
	}

	// Draw scene objects
	if (sFloor) sFloor->draw();
	if (sTower) sTower->draw();
	if (sControlledBoid) sControlledBoid->draw();
	if (sFlock) sFlock->draw();

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

static void keyboardControl(unsigned char key, int x, int y)
{
	// Use WASDQE keys to control the player boid:
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
	case 'a': case 'A': // Rotate left
		sControlledBoid->rotateYaw(rotateAmount);
		break;
	case 'd': case 'D': // Rotate right
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

		// Camera switching
	case '1': // Follow camera
		sCurrentCamera = FOLLOW_CAMERA;
		break;
	case '2': // Fixed camera
		sCurrentCamera = FIXED_CAMERA;
		break;
	case '3': // Side camera
		sCurrentCamera = SIDE_CAMERA;
		break;

		// Exit
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
	// Use arrow keys to control the special player boid:
	const double rotateAmount = 5.0; // degrees per key press
	if (!sControlledBoid) return;

	switch (key)
	{
	case GLUT_KEY_LEFT:		// Rotate left
		sControlledBoid->rotateYaw(rotateAmount);
		break;
	case GLUT_KEY_RIGHT:	// Rotate right
		sControlledBoid->rotateYaw(-rotateAmount);
		break;
	case GLUT_KEY_UP:		// Accelerate forward
		sControlledBoid->moveForward(1.0);
		break;
	case GLUT_KEY_DOWN:		// Decelerate / move backward
		sControlledBoid->moveBackward(1.0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

static void mouseFunc(int button, int state, int x, int y)
{
	// Mouse callback: handle wheel as button 3 (up) / 4 (down)
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
	
	default:
		break;
	}
}

static void idle(void) { glutPostRedisplay(); }

void registerWorldObjects(
	Camera& followCamera,
	Camera& fixedCamera,
	Camera& sideCamera,
	Flock& flock,
	ControlledBoid& controlledBoid,
	Floor& floor,
	Tower& tower)
{
	sFollowCamera = &followCamera;
	sFixedCamera = &fixedCamera;
	sSideCamera = &sideCamera;
	sFlock = &flock;
	sControlledBoid = &controlledBoid;
	sFloor = &floor;
	sTower = &tower;
}

/* End of GLUT callback Handlers */
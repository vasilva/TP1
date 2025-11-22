#include <GL\glut.h>
#include <vector>

#include "glut_callback.h"
#include "Camera.h"
#include "Tower.h"
#include "Floor.h"
#include "Obstacle.h"
#include "Flock.h"
#include "ControlledBoid.h"
#include "ObstacleManager.h"
#include "vecFunctions.h"
#include "World.h"

// Lighting parameters
const GLfloat light_ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };	 // Ambient light
const GLfloat light_diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };	 // Diffuse light
const GLfloat light_specular[4] = { 0.3f, 0.3f, 0.3f, 1.0f };	 // Specular light
const GLfloat light_position[4] = { 0.0f, 100.0f, 10.0f, 0.0f }; // Light position

// Material parameters
const GLfloat mat_ambient[4] = { 0.6f, 0.6f, 0.6f, 1.0f };  // Material ambient reflectance
const GLfloat mat_diffuse[4] = { 0.7f, 0.7f, 0.7f, 1.0f };  // Material diffuse reflectance
const GLfloat mat_specular[4] = { 0.2f, 0.2f, 0.2f, 1.0f }; // Material specular reflectance
const GLfloat high_shininess[1] = { 30.0f };

int main(int argc, char* argv[])
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1920, 1080);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("TP1 - boids");
	glutFullScreen();

	// Set background color to cyan
	Vec3 backgroundColor = Color::Cyan;
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);

	// Create controlled boid
	ControlledBoid controlledBoid;
	controlledBoid.setPosition(20.0f, 10.0f, 20.0f);
	controlledBoid.setSize(0.5f, 0.1f, 0.5f);

	// Create floor
	Floor floor;
	floor.setPosition(Zero);
	floor.setRotation(Zero);
	floor.setSize(1000.0f, 1.0f, 1000.0f);
	auto floorSize = floor.getSize();

	// Create tower at the center of the floor
	Tower tower;
	tower.setPosition(Zero);
	tower.setRotation(UnitX * -90.0f);
	tower.setSize(10.0f, 100.0f, 10.0f);
	gWorldTower = &tower;

	// Create several obstacles scattered over the floor
	std::vector<Obstacle> walls;
	ObstacleManager obstacleManager;
	obstacleManager.setFloor(&floor);
	obstacleManager.generateRandom(100);

	// Create and initialize flock
	Flock flock;
	flock.init(50, &controlledBoid, floorSize.x * 0.2f);

	// Initialize cameras
	Camera followCamera, fixedCamera, sideCamera;

	// Register GLUT callbacks
	registerWorldObjects(
		followCamera, fixedCamera, sideCamera,
		flock, controlledBoid,
		floor, tower);
	registerObstacleManager(obstacleManager);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboardControl);
	glutSpecialFunc(cameraSpecial);
	glutMouseFunc(mouseFunc);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Enable culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	// Lightning settings
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	// Material settings
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

	// Start GLUT main loop
	glutMainLoop();
	return 0;
}

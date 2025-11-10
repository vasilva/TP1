#include <GL\glut.h>
#include <vector>
#include <random>
#include "glut_callback.h"
#include "Camera.h"
#include "Tower.h"
#include "Floor.h"
#include "Obstacle.h"
#include "Flock.h"
#include "ControlledBoid.h"

// Lighting parameters
const GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };		// Ambient light
const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };		// Diffuse light
const GLfloat light_specular[] = { 0.3f, 0.3f, 0.3f, 1.0f };	// Specular light
const GLfloat light_position[] = { 0.0f, 100.0f, 20.0f, 0.0f };	// Light position

// Material parameters
const GLfloat mat_ambient[] = { 0.6f, 0.6f, 0.6f, 1.0f };  // Material ambient reflectance
const GLfloat mat_diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };  // Material diffuse reflectance
const GLfloat mat_specular[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // Material specular reflectance
const GLfloat high_shininess[] = { 30.0f };

// Function to create random walls (obstacles) on the floor
static void makeWalls(std::vector<Obstacle>& walls, const Floor& floor, int obstacleCount)
{
	auto floorSize = floor.getSize();
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<GLdouble> distPos(-floorSize.x * 0.5, floorSize.z * 0.5);
	std::uniform_real_distribution<GLdouble> distSize(5.0, 30.0);

	if (obstacleCount <= 0)
		obstacleCount = 50; // Default number of obstacles
	if (obstacleCount > 200)
		obstacleCount = 200; // Max limit

	for (int i = 0; i < obstacleCount; ++i)
	{
		// Random size
		GLdouble s = distSize(gen);
		Vec3 size = { s, s * 2.0, distSize(gen) };

		// Random position
		// Ensure obstacles are not too close to the center
		GLdouble px, pz;
		px = distPos(gen);
		pz = distPos(gen);

		if (std::abs(px) < 20.0) px += (px >= 0.0) ? 20.0 : -20.0;
		if (std::abs(pz) < 20.0) pz += (pz >= 0.0) ? 20.0 : -20.0;

		Vec3 pos = { px, size.y * 0.5, pz };
		walls.emplace_back(pos, size);
		walls.back().enableCollision();
	}
}

int main(int argc, char* argv[]) {

	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1920, 1080);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("TP1 - boids");
	glutFullScreen();

	Vec3 backgroundColor(0.3, 0.9, 0.9); // Cyan background
	glClearColor(static_cast<GLfloat>(backgroundColor.x),
		static_cast<GLfloat>(backgroundColor.y),
		static_cast<GLfloat>(backgroundColor.z),
		1.0f);

	// Create controlled boid
	ControlledBoid controlledBoid;
	controlledBoid.setPosition(20.0, 10.0, 20.0);
	controlledBoid.setSize(0.5, 0.1, 0.5);

	// Create floor
	Floor floor;
	floor.setPosition(Zero);
	floor.setSize(1000.0, 1.0, 1000.0);
	floor.setRotation(Zero);
	auto floorSize = floor.getSize();

	// Create tower at the center of the floor
	Tower tower;
	tower.setPosition(Zero);
	tower.setSize(10.0, 100.0, 10.0);
	tower.setRotation(UnitX * -90.0);
	gWorldTower = &tower;

	// Create several obstacles scattered over the floor
	std::vector<Obstacle> walls;
	gWorldObstacles = &walls;
	makeWalls(walls, floor, 100);

	// Initialize flock
	Flock flock;
	flock.init(50, &controlledBoid, floorSize.x * 0.2);

	// Initialize cameras
	Camera followCamera, fixedCamera, sideCamera;

	// Register GLUT callbacks
	registerWorldObjects(
		followCamera, fixedCamera, sideCamera,
		flock, controlledBoid,
		floor, tower, walls);
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
	return EXIT_SUCCESS;
}

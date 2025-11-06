#include <GL\glut.h>
#include <iostream>

#include "glut_callback.h"
#include "Camera.h"
#include "Tower.h"
#include "Floor.h"
#include "Boid.h"

const GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };	   // Low ambient light
const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };	   // White diffuse light
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };   // White specular light
const GLfloat light_position[] = { 0.0f, 20.0f, 20.0f, 0.0f }; // Light position

const GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };  // Material ambient reflectance
const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };  // Material diffuse reflectance
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Material specular reflectance
const GLfloat high_shininess[] = { 100.0f };

int main(int argc, char* argv[]) {

	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitWindowSize(1920, 1080);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("TP1 - boids");

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f); // Gray background

	// Initialize camera
	Camera camera;
	camera.applyView();
	camera.setPosition(0.0, 10.0, 20.0);
	camera.setTarget(Zero);

	// Create objects
	Floor floor;
	floor.setPosition(UnitZ * globalZoom);
	floor.setSize(200.0, 0.5, 200.0);
	
	Tower tower;
	tower.setPosition(0.0, 0.1, globalZoom);
	tower.setSize(1.0, 5.0, 1.0);
	
	Boid player;


	// Register GLUT callbacks
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	registerCameraCallbacks(camera);

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

	glutMainLoop();
	return 0;
}
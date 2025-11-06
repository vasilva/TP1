#include <GL/glut.h>
#include <cmath>
#include "Boid.h"

Boid::Boid()
	: maxSpeed(8.0), maxForce(4.0),
	neighRadius(10.0), separationRadius(3.0),
	weightCohesion(1.0), weightSeparation(1.5), weightAlignment(1.0)
{
	setPosition(0.0, 0.0, 0.0);
	setVelocity(0.0, 0.0, 0.0);
	setSize(0.5, 0.5, 0.5);
}

Boid::Boid(const Vec3 pos) : Boid()
{
	setPosition(pos);
}

void Boid::update(const std::vector<Boid*>& boids, GLdouble deltaTime)
{
	applyBehaviors(boids, deltaTime);

	// Update position based on velocity
	auto newPos = Vec3(getPosition() + getVelocity() * deltaTime);
	setPosition(newPos);
}

void Boid::draw()
{
	// Draw the boid as a simple triangle (representing a bird)
	auto pos = getPosition();
	auto vel = getVelocity();
	auto dir = vel;
	auto len = length(dir);

	// Draw
	glPushMatrix();
	glTranslated(pos.x, pos.y, pos.z);
	if (len > 1e-6)
	{
		// Rotate to align with velocity direction
		auto yaw = std::atan2(dir.x, dir.z) * (180.0 / PI);
		auto pitch = std::asin(dir.y / len) * (180.0 / PI);
		glRotated(yaw, 0.0, 1.0, 0.0);
		glRotated(-pitch, 1.0, 0.0, 0.0);
	}
	auto color = Vec3(0.8, 0.2, 0.2); // Reddish color
	glColor3d(color.x, color.y, color.z);
	glutSolidCone(0.2, 0.5, 8, 2);
	color = Color::Black;
	glColor3d(color.x, color.y, color.z);
	glutWireCone(0.2, 0.5, 8, 2);
	glPopMatrix();
}

void Boid::applyBehaviors(const std::vector<Boid*>& neighbors, GLdouble dt)
{
	Vec3 cohesion(Zero);
	Vec3 separation(Zero);
	Vec3 alignment(Zero);

	flock(neighbors, cohesion.x, cohesion.y, cohesion.z);
	separate(neighbors, separation.x, separation.y, separation.z);
	align(neighbors, alignment.x, alignment.y, alignment.z);

	// Apply weighted forces
	cohesion *= weightCohesion;
	separation *= weightSeparation;
	alignment *= weightAlignment;

	// Apply forces to velocity
	Vec3 steer(Zero);
	steer += cohesion;
	steer += separation;
	steer += alignment;
	limit(steer, maxForce);

	// Update velocity
	auto newVelocity = Vec3(getVelocity() + steer * dt);
	limit(newVelocity, maxSpeed);
	setVelocity(newVelocity);
}

void Boid::flock(const std::vector<Boid*>& neighbors, GLdouble& rx, GLdouble& ry, GLdouble& rz)
{
	// Cohesion behavior implementation
	Vec3 pos = getPosition();
	Vec3 center(Zero);
	int count = 0;
	
	for (auto b : neighbors)
	{
		if (b == this) continue; // Skip self
		auto otherPos = b->getPosition();
		GLdouble distance = length(otherPos - pos);
		if (distance > 0 && distance < neighRadius)
		{
			center += otherPos;
			count++;
		}
	}
	if (count == 0) 
	{
		rx = ry = rz = 0.0;
		return;
	}
	center /= static_cast<GLdouble>(count);

	// Desired velocity towards center
	Vec3 desired = center - pos;
	normalize(desired);
	desired *= maxSpeed;

	// Steering force
	Vec3 steer = desired - getVelocity();
	limit(steer, maxForce);
	rx = steer.x, ry = steer.y, rz = steer.z;
}

void Boid::separate(const std::vector<Boid*>& neighbors, GLdouble& rx, GLdouble& ry, GLdouble& rz)
{
	// Separation behavior implementation
	Vec3 pos = getPosition();
	Vec3 steer(Zero);
	int count = 0;

	for (auto b : neighbors)
	{
		if (b == this) continue; // Skip self
		auto otherPos = b->getPosition();
		Vec3 diff = pos - otherPos;
		GLdouble distance = length(diff);
		if (distance > 0 && distance < separationRadius)
		{
			steer += diff / distance;
			count++;
		}
	}
	if (count == 0) 
	{
		rx = ry = rz = 0.0;
		return;
	}
	steer /= static_cast<GLdouble>(count);
	normalize(steer);
	steer *= maxSpeed;

	// Steering force
	auto res = steer - getVelocity();
	limit(res, maxForce);
	rx = res.x, ry = res.y, rz = res.z;
}

void Boid::align(const std::vector<Boid*>& neighbors, GLdouble& rx, GLdouble& ry, GLdouble& rz)
{
	// Alignment behavior implementation
	Vec3 avgVelocity(Zero);
	int count = 0;
	for (auto b : neighbors)
	{
		if (b == this) continue; // Skip self
		auto otherPos = b->getPosition();
		auto otherVel = b->getVelocity();
		GLdouble distance = length(otherPos - getPosition());
		if (distance > 0 && distance < neighRadius)
		{
			avgVelocity += otherVel;
			count++;
		}
	}
	if (count == 0) 
	{
		rx = ry = rz = 0.0;
		return;
	}
	avgVelocity /= static_cast<GLdouble>(count);
	normalize(avgVelocity);
	avgVelocity *= maxSpeed;
	
	// Steering force
	Vec3 steer = avgVelocity - getVelocity();
	limit(steer, maxForce);
	rx = steer.x, ry = steer.y, rz = steer.z;
}


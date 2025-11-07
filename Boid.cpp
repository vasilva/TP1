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

	// Get current position and velocity
	const Vec3 pos = getPosition();
	const Vec3 vel = getVelocity();

	// Update position based on velocity
	auto newPos = pos + vel * deltaTime;
	newPos.y = pos.y; // Keep y constant for 2D plane movement
	setPosition(newPos);
}

void Boid::applyBehaviors(const std::vector<Boid*>& neighbors, GLdouble dt)
{
	Vec3 cohesion(Zero);
	Vec3 separation(Zero);
	Vec3 alignment(Zero);

	flock(neighbors, cohesion.x, cohesion.y, cohesion.z);
	separate(neighbors, separation.x, separation.y, separation.z);
	align(neighbors, alignment.x, alignment.y, alignment.z);

	// Force the y-component to be zero for 2D plane movement
	cohesion.y = 0.0;
	separation.y = 0.0;
	alignment.y = 0.0;

	// Apply weighted forces
	cohesion *= weightCohesion;
	separation *= weightSeparation;
	alignment *= weightAlignment;

	// Apply forces to velocity
	Vec3 steer(Zero);
	steer += cohesion;
	steer += separation;
	steer += alignment;
	steer.y = 0.0; // Keep y-component zero
	limit(steer, maxForce);

	// Update velocity
	auto newVelocity = Vec3(getVelocity() + steer * dt);
	limit(newVelocity, maxSpeed);
	newVelocity.y = 0.0; // Keep y-component zero
	setVelocity(newVelocity);
}

void Boid::draw()
{
	// Draw the boid as a simple triangle (representing a bird)
	Vec3 pos = getPosition();
	Vec3 vel = getVelocity();
	Vec3 dir = { vel.x, 0.0, vel.z };
	auto len = length(dir);

	// Draw
	glPushMatrix();
	glTranslated(pos.x, pos.y, pos.z);
	if (len > 1e-6)
	{
		// Rotate to align with velocity direction
		auto yaw = std::atan2(dir.x, dir.z) * (180.0 / PI);
		glRotated(yaw, 0.0, 1.0, 0.0);
	}
	auto color = Color::Yellow;
	glColor3d(color.x, color.y, color.z);
	glutSolidCone(0.2, 0.5, 8, 2);

	color = Color::Black;
	glColor3d(color.x, color.y, color.z);
	glutWireCone(0.2, 0.5, 8, 2);
	glPopMatrix();
}

void Boid::flock(const std::vector<Boid*>& neighbors, GLdouble& rx, GLdouble& ry, GLdouble& rz)
{
	// Cohesion behavior implementation
	Vec3 pos = getPosition();
	Vec3 center = Zero;
	int count = 0;
	for (auto b : neighbors)
	{
		if (b == this) continue;
		const Vec3 otherPos = b->getPosition();
		// distance in XZ plane
		GLdouble dx = otherPos.x - pos.x, dz = otherPos.z - pos.z;
		GLdouble d = std::sqrt(dx * dx + dz * dz);
		if (d > 0 && d < neighRadius) {
			center.x += otherPos.x;
			center.z += otherPos.z;
			++count;
		}
	}
	if (count == 0)
	{
		rx = ry = rz = 0.0;
		return;
	}
	center.x /= count;
	center.z /= count;
	center.y = pos.y; // keep y the same

	// desired = center - position (only XZ)
	Vec3 desired = { center.x - pos.x, 0.0, center.z - pos.z };
	normalize(desired);
	desired *= maxSpeed;

	const Vec3 vel = getVelocity();
	Vec3 steer = { desired.x - vel.x, 0.0, desired.z - vel.z };
	limit(steer, maxForce);
	rx = steer.x; ry = 0.0; rz = steer.z;
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
		GLdouble dx = pos.x - otherPos.x, dz = pos.z - otherPos.z;
		GLdouble d = std::sqrt(dx * dx + dz * dz);
		if (d > 0 && d < separationRadius)
		{
			// Calculate vector pointing away from neighbor
			steer.x += dx / d;
			steer.z += dz / d;
			count++;
		}
	}
	if (count == 0)
	{
		rx = ry = rz = 0.0;
		return;
	}
	steer.x /= static_cast<GLdouble>(count);
	steer.z /= static_cast<GLdouble>(count);

	normalize(steer);
	steer.x *= maxSpeed;
	steer.z *= maxSpeed;

	// Steering force
	auto res = steer - getVelocity();
	res.y = 0.0; // Keep y-component zero
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
		GLdouble dx = otherPos.x - getPosition().x, dz = otherPos.z - getPosition().z;
		GLdouble distance = std::sqrt(dx * dx + dz * dz);
		if (distance > 0 && distance < neighRadius)
		{
			avgVelocity.x += otherVel.x;
			avgVelocity.z += otherVel.z;
			count++;
		}
	}
	if (count == 0)
	{
		rx = ry = rz = 0.0;
		return;
	}
	avgVelocity /= static_cast<GLdouble>(count);
	avgVelocity.y = 0.0; // Keep y-component zero
	normalize(avgVelocity);
	avgVelocity *= maxSpeed;

	// Steering force
	Vec3 steer = avgVelocity - getVelocity();
	steer.y = 0.0; // Keep y-component zero
	limit(steer, maxForce);
	rx = steer.x, ry = steer.y, rz = steer.z;
}

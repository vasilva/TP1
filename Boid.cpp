#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include "vecFunctions.h"
#include "Boid.h"
#include "World.h"
#include "ControlledBoid.h"
#include "Obstacle.h"
#include "Tower.h"

Boid::Boid()
	: maxSpeed(50.0), maxForce(30.0), yaw(0.0),
	neighRadius(10.0), separationRadius(10.0),
	weightCohesion(1.0), weightSeparation(2.0), weightAlignment(1.0)
{
	setPosition(Zero);
	setVelocity(Zero);
	setSize(0.5, 0.5, 0.5);
	setColors(Color::Red, Color::Orange, Color::Yellow);
}

Boid::Boid(const Vec3 pos, ControlledBoid* leader) : Boid()
{
	setPosition(pos);
	setLeader(leader);
}

// Update boid position and velocity based on behaviors
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

// Apply flocking behaviors: cohesion, separation, alignment, obstacle avoidance and leader following
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

	const GLdouble obstacleWeight = 40.0;	// Weight for obstacle avoidance
	const GLdouble safetyPadding = 3.0;		// Extra distance to avoid obstacles

	// Obstacle avoidance
	Vec3 obstacleAvoid(Zero);
	if (gWorldObstacles && !gWorldObstacles->empty())
	{
		// Check each obstacle for potential collision
		int avoidCount = 0;
		Vec3 myPos = getPosition();
		for (auto& obs : *gWorldObstacles)
		{
			// Skip non-collidable obstacles
			if (!obs.canCollide()) continue;
			
			// Obstacle position and size
			Vec3 obsPos = obs.getPosition();
			Vec3 obsSize = obs.getSize();

			// AABB check in XZ plane
			GLdouble halfWidth = obsSize.x * 0.5 + separationRadius + safetyPadding;
			GLdouble halfDepth = obsSize.z * 0.5 + separationRadius + safetyPadding;
			if (std::abs(myPos.x - obsPos.x) > halfWidth ||
				std::abs(myPos.z - obsPos.z) > halfDepth)
				continue; // No threat
			
			// Use squared distance to avoid sqrt for performance
			GLdouble dx = myPos.x - obsPos.x;
			GLdouble dz = myPos.z - obsPos.z;
			GLdouble dist2 = dx * dx + dz * dz;

			// obstacle radius approximate
			GLdouble radius = std::max(std::max(obsSize.x, obsSize.z) * 0.5, 1.0);
			GLdouble threatRadius = radius + separationRadius + safetyPadding;
			GLdouble threatRadius2 = threatRadius * threatRadius;
			if (dist2 > 0.0 && dist2 < threatRadius2)
			{
				// Calculate avoidance strength (non-linear)
				GLdouble dist = std::sqrt(dist2);
				GLdouble normalized = (threatRadius - dist) / threatRadius;
				GLdouble strength = normalized * normalized;
				if (dist < radius * 0.5) strength = std::min(1.0, strength * 3.0);
				
				// Direction away from obstacle
				Vec3 away = { dx, 0.0, dz };
				normalize(away);
				obstacleAvoid += away * (strength * obstacleWeight * maxSpeed);
				++avoidCount;
			}
		}
		// Average the avoidance force
		if (avoidCount > 0) obstacleAvoid /= static_cast<GLdouble>(avoidCount);
	}

	// Tower avoidance
	Vec3 towerAvoid(Zero);
	if (gWorldTower && gWorldTower->canCollide())
	{
		// Tower position
		Vec3 towerPos = gWorldTower->getPosition();
		Vec3 myPos = getPosition();
		
		// Use squared distance to avoid sqrt for performance
		GLdouble dx = myPos.x - towerPos.x;
		GLdouble dz = myPos.z - towerPos.z;
		GLdouble dist2 = dx * dx + dz * dz;

		// tower radius approximate
		Vec3 towerSize = gWorldTower->getSize();
		GLdouble radius = std::max(std::max(towerSize.x, towerSize.z) * 0.5, 1.0);
		GLdouble threatRadius = radius + separationRadius + safetyPadding;

		if (dist2 > 0.0 && dist2 < threatRadius * threatRadius)
		{
			// Calculate distance
			Vec3 away = { dx, 0.0, dz };
			normalize(away);

			// Calculate avoidance strength (non-linear)
			GLdouble dist = std::sqrt(dist2);
			GLdouble normalized = (threatRadius - dist) / threatRadius;
			GLdouble strength = normalized * normalized;

			// Boost strength if very close to tower
			const GLdouble towerCloseBoost = 3.0;
			if (dist < radius * 0.5) strength = std::min(1.0, strength * towerCloseBoost);

			// Apply tower avoidance force
			const GLdouble towerWeight = obstacleWeight * 1.2;
			towerAvoid = away * (strength * towerWeight * maxSpeed);
		}
	}

	// Leader following
	Vec3 leaderAttract(Zero);
	if (leaderBoid)
	{
		// Positions
		Vec3 myPos = getPosition();
		Vec3 leaderPos = leaderBoid->getPosition();
		
		// Direction the leader is facing
		GLdouble leaderYaw = leaderBoid->getYaw() * (PI / 180.0);
		Vec3 leaderForward = { std::sin(leaderYaw), 0.0, std::cos(leaderYaw) };
		normalize(leaderForward);
		
		// Desired follow distance based on leader speed	
		const GLdouble baseFollowDist = 5.0; 
		
		// Leader speed
		Vec3 leaderVel = leaderBoid->getVelocity();
		GLdouble leaderSpeed = length(leaderVel);
		GLdouble followDist = baseFollowDist + leaderSpeed * 0.5;

		// Calculate follow target position behind the leader
		Vec3 followTarget = leaderPos - leaderForward * followDist;
		
		// Keep same height
		followTarget.y = myPos.y;

		// Vector to the follow target
		Vec3 toTarget = { followTarget.x - myPos.x, 0.0, followTarget.z - myPos.z };
		GLdouble dist = length(toTarget);
		if (dist > 0.001)
		{
			normalize(toTarget);

			// desired velocity na direção do target
			const GLdouble leaderWeight = 1.0;     
			const GLdouble leaderInfluenceRadius = 500.0; 
			GLdouble influence = 1.0;
			
			// falloff based on distance
			if (dist > leaderInfluenceRadius) influence = 0.0;
			// linear falloff
			else influence = 1.0 - (dist / leaderInfluenceRadius);
			
			// Calculate steering force towards follow target
			Vec3 desired = toTarget * maxSpeed;
			Vec3 steerLeader = { desired.x - getVelocity().x, 0.0, desired.z - getVelocity().z };
			leaderAttract = steerLeader * (leaderWeight * influence);
		}
	}
	// Combine all steering forces
	Vec3 steer = cohesion + separation + alignment + obstacleAvoid + towerAvoid + leaderAttract;
	steer.y = 0.0;
	limit(steer, maxForce);

	// Update velocity
	Vec3 newVel = getVelocity() + steer * dt;
	newVel.y = 0.0;
	limit(newVel, maxSpeed);
	setVelocity(newVel);
}

// Draw the boid
void Boid::draw()
{
	auto pos = getPosition();
	auto rotation = getRotation();
	auto size = getSize();

	auto frontColor = getFrontColor();
	auto bodyColor = getBodyColor();
	auto wingColor = getWingColor();
	auto wireColor = getWireColor();

	yaw = std::atan2(getVelocity().x, getVelocity().z) * (180.0 / PI);

	// Transformations
	glPushMatrix();
	glTranslated(pos.x, pos.y, pos.z);
	glRotated(rotation.x, 1.0, 0.0, 0.0);
	glRotated(rotation.y + yaw, 0.0, 1.0, 0.0);
	glRotated(rotation.z, 0.0, 0.0, 1.0);

	// Front cone
	glScaled(size.x, size.y, size.z);
	glColor3d(frontColor.x, frontColor.y, frontColor.z);
	glutSolidCone(1.0, 1.0, 6, 1);
	glColor3d(wireColor.x, wireColor.y, wireColor.z);
	glutWireCone(1.0, 1.0, 6, 1);

	// Body
	glRotated(180.0, 1.0, 0.0, 0.0);
	glColor3d(bodyColor.x, bodyColor.y, bodyColor.z);
	glutSolidCone(1.0, 2.0, 6, 1);
	glColor3d(wireColor.x, wireColor.y, wireColor.z);
	glutWireCone(1.0, 2.0, 6, 1);

	// Wings
	glTranslated(0.0, 0.0, size.z);
	glRotated(180.0, 1.0, 0.0, 0.0);
	glScaled(1.0, 0.5, 1.0);
	glColor3d(wingColor.x, wingColor.y, wingColor.z);
	glutSolidCone(2.0, 1.0, 6, 1);
	glColor3d(wireColor.x, wireColor.y, wireColor.z);
	glutWireCone(2.0, 1.0, 6, 1);

	// Tail
	glTranslated(0.0, 0.0, -size.z * 3.0);
	glColor3d(wingColor.x, wingColor.y, wingColor.z);
	glutSolidCone(0.5, 1.0, 6, 1);
	glColor3d(wireColor.x, wireColor.y, wireColor.z);
	glutWireCone(0.5, 1.0, 6, 1);
	glPopMatrix();
}

void Boid::setColors(const Vec3 front, const Vec3 body, const Vec3 wing)
{
		frontColor = front;
		bodyColor = body;
		wingColor = wing;	
}

// Cohesion behavior: steer towards average position of neighbors
void Boid::flock(const std::vector<Boid*>& neighbors, GLdouble& rx, GLdouble& ry, GLdouble& rz)
{
	Vec3 pos = getPosition();
	Vec3 center = Zero;
	int count = 0;
	for (auto b : neighbors)
	{
		if (b == this) continue; // Skip self
		const Vec3 otherPos = b->getPosition();
		// distance in XZ plane
		GLdouble dx = otherPos.x - pos.x, dz = otherPos.z - pos.z;
		GLdouble d2 = dx * dx + dz * dz;
		if (d2 > 0.0 && d2 < neighRadius * neighRadius) {
			center.x += otherPos.x;
			center.z += otherPos.z;
			++count;
		}
	}
	// No neighbors
	if (count == 0)
	{
		rx = ry = rz = 0.0;
		return;
	}
	// Average position
	center.x /= count;
	center.z /= count;
	center.y = pos.y;

	// desired = center - position (only XZ)
	Vec3 desired = { center.x - pos.x, 0.0, center.z - pos.z };
	normalize(desired);
	desired *= maxSpeed;

	// Steering force
	const Vec3 vel = getVelocity();
	Vec3 steer = { desired.x - vel.x, 0.0, desired.z - vel.z };
	limit(steer, maxForce);
	rx = steer.x; ry = 0.0; rz = steer.z;
}

// Separation behavior: steer to avoid crowding neighbors
void Boid::separate(const std::vector<Boid*>& neighbors, GLdouble& rx, GLdouble& ry, GLdouble& rz)
{
	Vec3 pos = getPosition();
	Vec3 steer(Zero);
	int count = 0;
	for (auto b : neighbors)
	{
		if (b == this) continue; // Skip self
		auto otherPos = b->getPosition();
		GLdouble dx = pos.x - otherPos.x, dz = pos.z - otherPos.z;
		GLdouble d2 = dx * dx + dz * dz;
		if (d2 > 0.0 && d2 < separationRadius * separationRadius)
		{
			// Calculate vector pointing away from neighbor
			steer.x += dx / d2; // Weight by inverse distance
			steer.z += dz / d2;
			count++;
		}
	}
	// No neighbors too close
	if (count == 0)
	{
		rx = ry = rz = 0.0;
		return;
	}
	// Average the steer vector
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

// Alignment behavior: steer towards average heading of neighbors
void Boid::align(const std::vector<Boid*>& neighbors, GLdouble& rx, GLdouble& ry, GLdouble& rz)
{
	Vec3 avgVelocity(Zero);
	int count = 0;
	for (auto b : neighbors)
	{
		if (b == this) continue; // Skip self
		auto otherPos = b->getPosition();
		auto otherVel = b->getVelocity();
		GLdouble dx = otherPos.x - getPosition().x, dz = otherPos.z - getPosition().z;
		GLdouble d2 = dx * dx + dz * dz;
		if (d2 > 0.0 && d2 < neighRadius * neighRadius)
		{
			avgVelocity.x += otherVel.x;
			avgVelocity.z += otherVel.z;
			count++;
		}
	}
	// No neighbors
	if (count == 0)
	{
		rx = ry = rz = 0.0;
		return;
	}
	// Average velocity
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

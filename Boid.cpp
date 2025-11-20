#include <GL/glut.h>
#include <cmath>
#include <random>

#include "vecFunctions.h"
#include "Boid.h"
#include "World.h"
#include "ControlledBoid.h"
#include "Tower.h"
#include "Obstacle.h"
#include "Shadow.h"

Boid::Boid()
	: maxSpeed(50.0), maxForce(40.0), yaw(0.0),
	neighRadius(5.0), separationRadius(8.0),
	weightCohesion(1.0), weightSeparation(2.0), weightAlignment(1.0),
	wingAngle(0.0), wingAmplitude(30.0), wingBaseRate(8.0)
{
	setPosition(Zero);
	setVelocity(Zero);
	setSize(0.5, 0.5, 0.5);
	setColors(Color::Red, Color::Orange, Color::Yellow);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<GLdouble> dist(0.0, 2.0 * PI);
	wingAngle = dist(gen);

	auto size = getSize();

	noseLength = size.z * 0.6, noseRadius = size.x * 0.25;
	bodyLength = size.z * 1.2, bodyRadius = size.x * 0.3;
	tailLength = size.z * 0.5, tailRadius = size.x * 0.15;

	wingSpan = size.x * 0.9;
	wingChord = size.z * 0.6;
	wingThickness = size.y * 0.05;
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

	// Wing animation update
	GLdouble speed = length(getVelocity());
	GLdouble speedFactor = 0.0;
	if (maxSpeed > 1e-6) speedFactor = std::min(1.0, speed / maxSpeed);

	// Flap rate increases with speed (min 0.5x to max 2.0x)
	GLdouble flapRate = wingBaseRate * (0.5 + 1.5 * speedFactor);
	wingAngle += flapRate * deltaTime;

	// Get current position and velocity
	const Vec3 pos = getPosition();
	const Vec3 vel = getVelocity();

	// Update position based on velocity
	auto newPos = pos + vel * deltaTime;

	// Prevent falling below ground level
	if (newPos.y < 0.1) newPos.y = 0.1;

	setPosition(newPos);
}

// Apply flocking behaviors
void Boid::applyBehaviors(const std::vector<Boid*>& neighbors, GLdouble dt)
{
	Vec3 cohesion(Zero);
	Vec3 separation(Zero);
	Vec3 alignment(Zero);

	flock(neighbors, cohesion.x, cohesion.y, cohesion.z);
	separate(neighbors, separation.x, separation.y, separation.z);
	align(neighbors, alignment.x, alignment.y, alignment.z);

	// Force zero y for planar behaviour
	cohesion.y = 0.0;
	separation.y = 0.0;
	alignment.y = 0.0;

	// Apply weights
	cohesion *= weightCohesion;
	separation *= weightSeparation;
	alignment *= weightAlignment;

	// Obstacle avoidance
	const GLdouble obstacleWeight = 10.0;
	const GLdouble safetyPadding = 1.0;

	Vec3 obstacleAvoid(Zero);
	if (gWorldObstacles && !gWorldObstacles->empty())
	{
		int avoidCount = 0;
		Vec3 myPos = getPosition();

		for (auto& obs : *gWorldObstacles)
		{
			if (!obs.canCollide()) continue;
			Vec3 obsPos = obs.getPosition();
			Vec3 obsSize = obs.getSize();

			// Obstacle AABB in XZ plane
			GLdouble halfX = obsSize.x * 0.5 + separationRadius + safetyPadding;
			GLdouble halfZ = obsSize.z * 0.5 + separationRadius + safetyPadding;

			// AABB min and max
			GLdouble minX = obsPos.x - halfX;
			GLdouble maxX = obsPos.x + halfX;
			GLdouble minZ = obsPos.z - halfZ;
			GLdouble maxZ = obsPos.z + halfZ;

			// AABB rejection test
			if (myPos.x < minX && myPos.x < obsPos.x - (halfX + separationRadius)) continue;
			if (myPos.x > maxX && myPos.x > obsPos.x + (halfX + separationRadius)) continue;
			if (myPos.z < minZ && myPos.z < obsPos.z - (halfZ + separationRadius)) continue;
			if (myPos.z > maxZ && myPos.z > obsPos.z + (halfZ + separationRadius)) continue;

			// Closest point on AABB to boid (XZ)
			GLdouble closestX = myPos.x;
			if (closestX < minX) closestX = minX;
			if (closestX > maxX) closestX = maxX;
			GLdouble closestZ = myPos.z;
			if (closestZ < minZ) closestZ = minZ;
			if (closestZ > maxZ) closestZ = maxZ;

			// Vector from obstacle surface (closest point) to boid in XZ
			GLdouble dx = myPos.x - closestX;
			GLdouble dz = myPos.z - closestZ;
			GLdouble dist2 = dx * dx + dz * dz;

			// Approximate circular threat radius (for smooth falloff)
			GLdouble approxRadius = std::max(std::max(obsSize.x, obsSize.z) * 0.5, 1.0) + separationRadius + safetyPadding;
			GLdouble approxRadius2 = approxRadius * approxRadius;

			// If inside inflated AABB (dist2 == 0) or within approx radius, compute avoidance
			if (dist2 == 0.0 || dist2 < approxRadius2)
			{
				Vec3 away;
				GLdouble dist = 0.0;
				if (dist2 == 0.0)
				{
					// Boid is inside the inflated AABB; push directly away from obstacle center in XZ
					away = { myPos.x - obsPos.x, 0.0, myPos.z - obsPos.z };
					// fallback if exactly coincident
					if (length2(away) < 1e-9)
						away = Vec3(1.0, 0.0, 0.0);

					normalize(away);
					dist = 0.0;
				}
				else
				{
					away = { dx, 0.0, dz };
					dist = std::sqrt(dist2);
					normalize(away);
				}

				// Strength: stronger if inside AABB or very close, smooth falloff otherwise
				GLdouble normalized = 0.0;
				if (dist == 0.0)
					normalized = 1.0; // maximum repulsion if inside box
				else
					normalized = (approxRadius - dist) / approxRadius; // 0..1

				// non-linear scaling to make force ramp up quickly when near/inside
				GLdouble strength = normalized * normalized;
				if (dist < (std::max(obsSize.x, obsSize.z) * 0.25 + 0.001))
					strength = std::min(1.0, strength * 3.0);

				// Compose avoidance vector (scale by obstacleWeight and boid's maxSpeed)
				obstacleAvoid += away * (strength * obstacleWeight * maxSpeed);
				++avoidCount;
			}
			// Average avoidance if multiple obstacles
			if (avoidCount > 0)
				obstacleAvoid /= static_cast<GLdouble>(avoidCount);
		}
		// Tower avoidance
		Vec3 towerAvoid(Zero);
		if (gWorldTower && gWorldTower->canCollide())
		{
			Vec3 towerPos = gWorldTower->getPosition();
			Vec3 myPos = getPosition();

			// Distance in XZ plane
			GLdouble dx = myPos.x - towerPos.x;
			GLdouble dz = myPos.z - towerPos.z;
			GLdouble dist = std::sqrt(dx * dx + dz * dz);

			// Radius of tower base
			Vec3 towerSize = gWorldTower->getSize();
			GLdouble radius = std::max(std::max(towerSize.x, towerSize.z) * 0.75, 1.0);

			// Threat radius
			GLdouble threatRadius = radius + separationRadius + safetyPadding;

			if (dist > 0.0 && dist < threatRadius)
			{
				// Direction away from tower in XZ
				Vec3 away = { dx, 0.0, dz };
				normalize(away);

				// Strength based on distance
				GLdouble normalized = (threatRadius - dist) / threatRadius; // 0..1
				GLdouble strength = normalized * normalized; // non-linear scaling

				// Boost strength if very close
				const GLdouble closeBoost = 3.0;
				if (dist < radius * 0.5)
					strength = std::min(1.0, strength * closeBoost);

				const GLdouble towerWeight = obstacleWeight * 1.8; // Stronger weight for tower
				towerAvoid = away * (strength * towerWeight * maxSpeed);
			}
		}
		// Leader following
		Vec3 leaderAttract(Zero);
		if (leaderBoid)
		{
			Vec3 myPos = getPosition();
			Vec3 leaderPos = leaderBoid->getPosition();

			// Vector to leader
			Vec3 toLeader = leaderPos - myPos;
			GLdouble dist = length(toLeader);

			// Only attract if beyond a small threshold
			if (dist > 0.001)
			{
				normalize(toLeader);
				Vec3 desired = toLeader * maxSpeed;
				leaderAttract = desired - getVelocity();
				limit(leaderAttract, maxForce);
			}
		}
		// Sum forces and limit
		Vec3 steer = cohesion + separation + alignment + obstacleAvoid + towerAvoid + leaderAttract;
		limit(steer, maxForce);

		// Update velocity
		Vec3 newVel = getVelocity() + steer * dt;
		limit(newVel, maxSpeed);
		setVelocity(newVel);
	}
}

void Boid::drawGeometry(bool useColor) const
{
	GLdouble flapDeg = wingAmplitude * std::sin(wingAngle);

	// --- NOSE ---
	glPushMatrix();
	if (useColor) glColor3d(frontColor.x, frontColor.y, frontColor.z);
	glTranslated(0.0, 0.0, noseLength * 0.5);
	glScaled(noseRadius, noseRadius, noseLength);
	glutSolidCone(1.0, 1.0, 8, 1);
	glPopMatrix();

	// --- BODY ---
	glPushMatrix();
	if (useColor) glColor3d(bodyColor.x, bodyColor.y, bodyColor.z);
	glScaled(bodyRadius, bodyRadius, bodyLength * 0.5);
	glutSolidSphere(1.0, 8, 8);
	glPopMatrix();

	// --- TAIL ---
	glPushMatrix();
	if (useColor) glColor3d(frontColor.x, frontColor.y, frontColor.z);
	glTranslated(0.0, 0.0, -bodyLength * 0.8);
	glScaled(tailRadius, tailRadius, tailLength);
	glutSolidCone(1.0, 1.0, 8, 1);
	glPopMatrix();

	// --- LEFT WING ---
	glPushMatrix();
	if (useColor) glColor3d(wingColor.x, wingColor.y, wingColor.z);
	glTranslated(wingSpan * 0.55, 0.0, 0.05);
	glRotated(flapDeg, 0.0, 0.0, 1.0);
	glTranslated(wingChord * 0.25, 0.0, -wingChord * 0.25);
	glScaled(wingSpan, wingThickness, wingChord);
	glutSolidCube(1.0);
	glPopMatrix();

	// --- RIGHT WING ---
	glPushMatrix();
	if (useColor) glColor3d(wingColor.x, wingColor.y, wingColor.z);
	glTranslated(-wingSpan * 0.55, 0.0, 0.05);
	glRotated(-flapDeg, 0.0, 0.0, 1.0);
	glTranslated(-wingChord * 0.25, 0.0, -wingChord * 0.25);
	glScaled(wingSpan, wingThickness, wingChord);
	glutSolidCube(1.0);
	glPopMatrix();
}

void Boid::drawShadow()
{
	// Get shadow matrix
	GLfloat plane[4] = { 0.0f, 1.0f, 0.0f, 0.0f }; // Ground plane y=0
	GLfloat light[4] = { 0.0f, 1000.0f, 10.0f, 1.0f }; // Positional light
	GLfloat S[16];
	computeShadowMatrix(S, plane, light);

	// Attributes for shadow
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	glColor4f(0.0f, 0.0f, 0.0f, 0.9f);

	auto pos = getPosition();
	auto rotation = getRotation();

	// Apply shadow matrix and draw
	glPushMatrix();
	glMultMatrixf(S);
	glTranslated(pos.x, pos.y, pos.z);
	glRotated(rotation.x, 1.0, 0.0, 0.0);
	glRotated(rotation.y + yaw, 0.0, 1.0, 0.0);
	glRotated(rotation.z, 0.0, 0.0, 1.0);
	drawGeometry(false);
	glPopMatrix();

	// Restore state
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glPopAttrib();
}

void Boid::drawBody()
{
	// Get position and rotation
	auto pos = getPosition();
	auto rotation = getRotation();
	GLdouble flapDeg = wingAmplitude * std::sin(wingAngle);

	// Transformations
	glPushMatrix();
	glTranslated(pos.x, pos.y, pos.z);
	glRotated(rotation.x, 1.0, 0.0, 0.0);
	glRotated(rotation.y + yaw, 0.0, 1.0, 0.0);
	glRotated(rotation.z, 0.0, 0.0, 1.0);

	// Draw geometry with colors
	drawGeometry(true);

	// Draw wireframe overlay
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glColor3d(wireColor.x, wireColor.y, wireColor.z);

	// Wire nose
	glPushMatrix();
	glTranslated(0.0, 0.0, noseLength * 0.5);
	glScaled(noseRadius, noseRadius, noseLength);
	glutWireCone(1.0, 1.0, 8, 1);
	glPopMatrix();

	// Wire body
	glPushMatrix();
	glScaled(bodyRadius, bodyRadius, bodyLength * 0.5);
	glutWireSphere(1.0, 8, 8);
	glPopMatrix();

	// Wire tail
	glPushMatrix();
	glTranslated(0.0, 0.0, -bodyLength * 0.8);
	glScaled(tailRadius, tailRadius, tailLength);
	glutWireCone(1.0, 1.0, 8, 1);
	glPopMatrix();

	// Wire left wing
	glPushMatrix();
	glTranslated(wingSpan * 0.55, 0.0, 0.05);
	glRotated(flapDeg, 0.0, 0.0, 1.0);
	glTranslated(wingChord * 0.25, 0.0, -wingChord * 0.25);
	glScaled(wingSpan, wingThickness, wingChord);
	glutWireCube(1.0);
	glPopMatrix();

	// Wire right wing
	glPushMatrix();
	glTranslated(-wingSpan * 0.55, 0.0, 0.05);
	glRotated(-flapDeg, 0.0, 0.0, 1.0);
	glTranslated(-wingChord * 0.25, 0.0, -wingChord * 0.25);
	glScaled(wingSpan, wingThickness, wingChord);
	glutWireCube(1.0);
	glPopMatrix();
	
	glPopAttrib();
	glPopMatrix();
}

// Draw the boid
void Boid::draw()
{
	// Determine yaw based on velocity
	Vec3 v = getVelocity();
	GLdouble speed = length(v);
	if (speed > 1e-6) yaw = std::atan2(v.x, v.z) * (180.0 / PI);

	// Draw shadow
	drawShadow();

	// Draw body
	glEnable(GL_LIGHTING);
	drawBody();
}

// Set body colors
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
		GLdouble dx = otherPos.x - pos.x;
		GLdouble dz = otherPos.z - pos.z;
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
		GLdouble dx = pos.x - otherPos.x;
		GLdouble dz = pos.z - otherPos.z;
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
		GLdouble dx = otherPos.x - getPosition().x;
		GLdouble dz = otherPos.z - getPosition().z;
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
	limit(steer, maxForce);
	rx = steer.x, ry = 0.0, rz = steer.z;
}

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
	: maxSpeed(50.0f), maxForce(40.0f), yaw(0.0f),
	neighRadius(5.0f), separationRadius(8.0f),
	weightCohesion(1.0f), weightSeparation(2.0f), weightAlignment(1.0f),
	wingAngle(0.0f), wingAmplitude(30.0f), wingBaseRate(8.0f)
{
	setPosition(Zero);
	setVelocity(Zero);
	setSize(One * 0.5f);
	setColors(Color::Red, Color::Orange, Color::Yellow);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<GLfloat> dist(0.0f, 2.0f * PI);
	wingAngle = dist(gen);

	auto size = getSize();

	noseLength = size.z * 0.6f, noseRadius = size.x * 0.25f;
	bodyLength = size.z * 1.2f, bodyRadius = size.x * 0.3f;
	tailLength = size.z * 0.5f, tailRadius = size.x * 0.15f;

	wingSpan = size.x * 0.9f;
	wingChord = size.z * 0.6f;
	wingThickness = size.y * 0.05f;
}

Boid::Boid(const Vec3 pos, ControlledBoid* leader) : Boid()
{
	setPosition(pos);
	setLeader(leader);
}

// Update boid position and velocity based on behaviors
void Boid::update(const std::vector<Boid*>& boids, GLfloat deltaTime)
{
	applyBehaviors(boids, deltaTime);

	// Wing animation update
	GLfloat speed = length(getVelocity());
	GLfloat speedFactor = 0.0f;
	if (maxSpeed > 1e-6) speedFactor = std::min(1.0f, speed / maxSpeed);

	// Flap rate increases with speed (min 0.5x to max 2.0x)
	GLfloat flapRate = wingBaseRate * (0.5f + 1.5f * speedFactor);
	wingAngle += flapRate * deltaTime;

	// Get current position and velocity
	const Vec3 pos = getPosition();
	const Vec3 vel = getVelocity();

	// Update position based on velocity
	auto newPos = pos + vel * deltaTime;

	// Prevent falling below ground level
	if (newPos.y < 0.1f) newPos.y = 0.1f;

	setPosition(newPos);
}

// Apply flocking behaviors
void Boid::applyBehaviors(const std::vector<Boid*>& neighbors, GLfloat dt)
{
	Vec3 cohesion(Zero);
	Vec3 separation(Zero);
	Vec3 alignment(Zero);

	flock(neighbors, cohesion.x, cohesion.y, cohesion.z);
	separate(neighbors, separation.x, separation.y, separation.z);
	align(neighbors, alignment.x, alignment.y, alignment.z);

	// Force zero y for planar behaviour
	cohesion.y = 0.0f;
	separation.y = 0.0f;
	alignment.y = 0.0f;

	// Apply weights
	cohesion *= weightCohesion;
	separation *= weightSeparation;
	alignment *= weightAlignment;

	// Obstacle avoidance
	const GLfloat obstacleWeight = 10.0f;
	const GLfloat safetyPadding = 1.0f;

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
			GLfloat halfX = obsSize.x * 0.5f + separationRadius + safetyPadding;
			GLfloat halfZ = obsSize.z * 0.5f + separationRadius + safetyPadding;

			// AABB min and max
			GLfloat minX = obsPos.x - halfX;
			GLfloat maxX = obsPos.x + halfX;
			GLfloat minZ = obsPos.z - halfZ;
			GLfloat maxZ = obsPos.z + halfZ;

			// AABB rejection test
			if (myPos.x < minX && myPos.x < obsPos.x - (halfX + separationRadius)) continue;
			if (myPos.x > maxX && myPos.x > obsPos.x + (halfX + separationRadius)) continue;
			if (myPos.z < minZ && myPos.z < obsPos.z - (halfZ + separationRadius)) continue;
			if (myPos.z > maxZ && myPos.z > obsPos.z + (halfZ + separationRadius)) continue;

			// Closest point on AABB to boid (XZ)
			GLfloat closestX = myPos.x;
			if (closestX < minX) closestX = minX;
			if (closestX > maxX) closestX = maxX;
			GLfloat closestZ = myPos.z;
			if (closestZ < minZ) closestZ = minZ;
			if (closestZ > maxZ) closestZ = maxZ;

			// Vector from obstacle surface (closest point) to boid in XZ
			GLfloat dx = myPos.x - closestX;
			GLfloat dz = myPos.z - closestZ;
			GLfloat dist2 = dx * dx + dz * dz;

			// Approximate circular threat radius (for smooth falloff)
			GLfloat approxRadius = std::max(std::max(obsSize.x, obsSize.z) * 0.5f, 1.0f) + separationRadius + safetyPadding;
			GLfloat approxRadius2 = approxRadius * approxRadius;

			// If inside inflated AABB (dist2 == 0) or within approx radius, compute avoidance
			if (dist2 == 0.0f || dist2 < approxRadius2)
			{
				Vec3 away;
				GLfloat dist = 0.0f;
				if (dist2 == 0.0f)
				{
					// Boid is inside the inflated AABB; push directly away from obstacle center in XZ
					away = { myPos.x - obsPos.x, 0.0f, myPos.z - obsPos.z };
					// fallback if exactly coincident
					if (length2(away) < 1e-9f)
						away = UnitX;

					normalize(away);
					dist = 0.0f;
				}
				else
				{
					away = { dx, 0.0f, dz };
					dist = std::sqrt(dist2);
					normalize(away);
				}

				// Strength: stronger if inside AABB or very close, smooth falloff otherwise
				GLfloat normalized = 0.0f;
				if (dist == 0.0f)
					normalized = 1.0f; // maximum repulsion if inside box
				else
					normalized = (approxRadius - dist) / approxRadius; // 0..1

				// non-linear scaling to make force ramp up quickly when near/inside
				GLfloat strength = normalized * normalized;
				if (dist < (std::max(obsSize.x, obsSize.z) * 0.25f + 0.001f))
					strength = std::min(1.0f, strength * 3.0f);

				// Compose avoidance vector (scale by obstacleWeight and boid's maxSpeed)
				obstacleAvoid += away * (strength * obstacleWeight * maxSpeed);
				++avoidCount;
			}
			// Average avoidance if multiple obstacles
			if (avoidCount > 0)
				obstacleAvoid /= static_cast<GLfloat>(avoidCount);
		}
		// Tower avoidance
		Vec3 towerAvoid(Zero);
		if (gWorldTower && gWorldTower->canCollide())
		{
			Vec3 towerPos = gWorldTower->getPosition();
			Vec3 myPos = getPosition();

			// Distance in XZ plane
			GLfloat dx = myPos.x - towerPos.x;
			GLfloat dz = myPos.z - towerPos.z;
			GLfloat dist = std::sqrt(dx * dx + dz * dz);

			// Radius of tower base
			Vec3 towerSize = gWorldTower->getSize();
			GLfloat radius = std::max(std::max(towerSize.x, towerSize.z) * 0.75f, 1.0f);

			// Threat radius
			GLfloat threatRadius = radius + separationRadius + safetyPadding;

			if (dist > 0.0f && dist < threatRadius)
			{
				// Direction away from tower in XZ
				Vec3 away = { dx, 0.0f, dz };
				normalize(away);

				// Strength based on distance
				GLfloat normalized = (threatRadius - dist) / threatRadius; // 0..1
				GLfloat strength = normalized * normalized; // non-linear scaling

				// Boost strength if very close
				const GLfloat closeBoost = 3.0f;
				if (dist < radius * 0.5f)
					strength = std::min(1.0f, strength * closeBoost);

				const GLfloat towerWeight = obstacleWeight * 1.8f; // Stronger weight for tower
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
			GLfloat dist = length(toLeader);

			// Only attract if beyond a small threshold
			if (dist > 0.001f)
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
	GLfloat flapDeg = wingAmplitude * std::sin(wingAngle);

	// --- NOSE ---
	glPushMatrix();
	if (useColor) glColor3f(frontColor.x, frontColor.y, frontColor.z);
	glTranslatef(0.0f, 0.0f, noseLength * 0.5f);
	glScalef(noseRadius, noseRadius, noseLength);
	glutSolidCone(1.0, 1.0, 8, 1);
	glPopMatrix();

	// --- BODY ---
	glPushMatrix();
	if (useColor) glColor3f(bodyColor.x, bodyColor.y, bodyColor.z);
	glScalef(bodyRadius, bodyRadius, bodyLength * 0.5f);
	glutSolidSphere(1.0, 8, 8);
	glPopMatrix();

	// --- TAIL ---
	glPushMatrix();
	if (useColor) glColor3f(frontColor.x, frontColor.y, frontColor.z);
	glTranslatef(0.0f, 0.0f, -bodyLength * 0.8f);
	glScalef(tailRadius, tailRadius, tailLength);
	glutSolidCone(1.0, 1.0, 8, 1);
	glPopMatrix();

	// --- LEFT WING ---
	glPushMatrix();
	if (useColor) glColor3f(wingColor.x, wingColor.y, wingColor.z);
	glTranslatef(wingSpan * 0.55f, 0.0f, 0.05f);
	glRotatef(flapDeg, 0.0f, 0.0f, 1.0f);
	glTranslatef(wingChord * 0.25f, 0.0f, -wingChord * 0.25f);
	glScalef(wingSpan, wingThickness, wingChord);
	glutSolidCube(1.0);
	glPopMatrix();

	// --- RIGHT WING ---
	glPushMatrix();
	if (useColor) glColor3f(wingColor.x, wingColor.y, wingColor.z);
	glTranslatef(-wingSpan * 0.55f, 0.0f, 0.05f);
	glRotatef(-flapDeg, 0.0f, 0.0f, 1.0f);
	glTranslatef(-wingChord * 0.25f, 0.0f, -wingChord * 0.25f);
	glScalef(wingSpan, wingThickness, wingChord);
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
	glDisable(GL_FOG);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	glColor4f(0.0f, 0.0f, 0.0f, 0.9f);

	auto pos = getPosition();
	auto rotation = getRotation();

	// Apply shadow matrix and draw
	glPushMatrix();
	glMultMatrixf(S);
	glTranslatef(pos.x, pos.y, pos.z);
	glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
	glRotatef(rotation.y + yaw, 0.0f, 1.0f, 0.0f);
	glRotatef(rotation.z, 0.0f, 0.0f, 1.0f);
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
	GLfloat flapDeg = wingAmplitude * std::sin(wingAngle);

	// Transformations
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
	glRotatef(rotation.y + yaw, 0.0f, 1.0f, 0.0f);
	glRotatef(rotation.z, 0.0f, 0.0f, 1.0f);

	// Draw geometry with colors
	drawGeometry(true);

	// Draw wireframe overlay
	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glColor3f(wireColor.x, wireColor.y, wireColor.z);

	// Wire nose
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, noseLength * 0.5f);
	glScalef(noseRadius, noseRadius, noseLength);
	glutWireCone(1.0, 1.0, 8, 1);
	glPopMatrix();

	// Wire body
	glPushMatrix();
	glScalef(bodyRadius, bodyRadius, bodyLength * 0.5f);
	glutWireSphere(1.0, 8, 8);
	glPopMatrix();

	// Wire tail
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -bodyLength * 0.8f);
	glScalef(tailRadius, tailRadius, tailLength);
	glutWireCone(1.0, 1.0, 8, 1);
	glPopMatrix();

	// Wire left wing
	glPushMatrix();
	glTranslatef(wingSpan * 0.55f, 0.0f, 0.05f);
	glRotatef(flapDeg, 0.0f, 0.0f, 1.0f);
	glTranslatef(wingChord * 0.25f, 0.0f, -wingChord * 0.25f);
	glScalef(wingSpan, wingThickness, wingChord);
	glutWireCube(1.0);
	glPopMatrix();

	// Wire right wing
	glPushMatrix();
	glTranslatef(-wingSpan * 0.55f, 0.0f, 0.05f);
	glRotatef(-flapDeg, 0.0f, 0.0f, 1.0f);
	glTranslatef(-wingChord * 0.25f, 0.0f, -wingChord * 0.25f);
	glScalef(wingSpan, wingThickness, wingChord);
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
	GLfloat speed = length(v);
	if (speed > 1e-6f) yaw = std::atan2(v.x, v.z) * (180.0f / PI);

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
void Boid::flock(const std::vector<Boid*>& neighbors, GLfloat& rx, GLfloat& ry, GLfloat& rz)
{
	Vec3 pos = getPosition();
	Vec3 center = Zero;
	int count = 0;
	for (auto b : neighbors)
	{
		if (b == this) continue; // Skip self
		const Vec3 otherPos = b->getPosition();
		// distance in XZ plane
		GLfloat dx = otherPos.x - pos.x;
		GLfloat dz = otherPos.z - pos.z;
		GLfloat d2 = dx * dx + dz * dz;
		if (d2 > 0.0f && d2 < neighRadius * neighRadius) {
			center.x += otherPos.x;
			center.z += otherPos.z;
			++count;
		}
	}
	// No neighbors
	if (count == 0)
	{
		rx = ry = rz = 0.0f;
		return;
	}
	// Average position
	center.x /= count;
	center.z /= count;
	center.y = pos.y;

	// desired = center - position (only XZ)
	Vec3 desired = { center.x - pos.x, 0.0f, center.z - pos.z };
	normalize(desired);
	desired *= maxSpeed;

	// Steering force
	const Vec3 vel = getVelocity();
	Vec3 steer = { desired.x - vel.x, 0.0f, desired.z - vel.z };
	limit(steer, maxForce);
	rx = steer.x; ry = 0.0f; rz = steer.z;
}

// Separation behavior: steer to avoid crowding neighbors
void Boid::separate(const std::vector<Boid*>& neighbors, GLfloat& rx, GLfloat& ry, GLfloat& rz)
{
	Vec3 pos = getPosition();
	Vec3 steer(Zero);
	int count = 0;
	for (auto b : neighbors)
	{
		if (b == this) continue; // Skip self
		auto otherPos = b->getPosition();
		GLfloat dx = pos.x - otherPos.x;
		GLfloat dz = pos.z - otherPos.z;
		GLfloat d2 = dx * dx + dz * dz;
		if (d2 > 0.0f && d2 < separationRadius * separationRadius)
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
		rx = ry = rz = 0.0f;
		return;
	}
	// Average the steer vector
	steer.x /= static_cast<GLfloat>(count);
	steer.z /= static_cast<GLfloat>(count);
	normalize(steer);
	steer.x *= maxSpeed;
	steer.z *= maxSpeed;

	// Steering force
	auto res = steer - getVelocity();
	res.y = 0.0f; // Keep y-component zero
	limit(res, maxForce);
	rx = res.x, ry = res.y, rz = res.z;
}

// Alignment behavior: steer towards average heading of neighbors
void Boid::align(const std::vector<Boid*>& neighbors, GLfloat& rx, GLfloat& ry, GLfloat& rz)
{
	Vec3 avgVelocity(Zero);
	int count = 0;
	for (auto b : neighbors)
	{
		if (b == this) continue; // Skip self
		auto otherPos = b->getPosition();
		auto otherVel = b->getVelocity();
		GLfloat dx = otherPos.x - getPosition().x;
		GLfloat dz = otherPos.z - getPosition().z;
		GLfloat d2 = dx * dx + dz * dz;
		if (d2 > 0.0f && d2 < neighRadius * neighRadius)
		{
			avgVelocity.x += otherVel.x;
			avgVelocity.z += otherVel.z;
			count++;
		}
	}
	// No neighbors
	if (count == 0)
	{
		rx = ry = rz = 0.0f;
		return;
	}
	// Average velocity
	avgVelocity /= static_cast<GLfloat>(count);
	avgVelocity.y = 0.0f; // Keep y-component zero
	normalize(avgVelocity);
	avgVelocity *= maxSpeed;

	// Steering force
	Vec3 steer = avgVelocity - getVelocity();
	limit(steer, maxForce);
	rx = steer.x, ry = 0.0f, rz = steer.z;
}

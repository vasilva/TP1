#pragma once
#include <vector>
#include "Object.h"
#include "vecFunctions.h"

class ControlledBoid;

// Boid class representing an individual boid in the flock
class Boid : public Object
{
public:
	Boid();
	Boid(const Vec3 pos, ControlledBoid* leader);

	// Boid behavior methods
	void update(const std::vector<Boid*>& boids, GLdouble deltaTime);
	void draw() override;
	void setLeader(ControlledBoid* leader) { leaderBoid = leader; }
	
	// Getters for movement attributes
	GLdouble getYaw() const { return yaw; }
	GLdouble getWingAngle() const { return wingAngle; }
	GLdouble getWingAmplitude() const { return wingAmplitude; }
	GLdouble getWingBaseRate() const { return wingBaseRate; }

	// Setters for movement attributes
	void setWingAngle(GLdouble angle) { wingAngle = angle; }
	void setWingAmplitude(GLdouble amplitude) { wingAmplitude = amplitude; }
	void setWingBaseRate(GLdouble rate) { wingBaseRate = rate; }

	// Set body colors
	void setColors(const Vec3 front, const Vec3 body, const Vec3 wing);

	// Getters for colors
	const Vec3 getFrontColor() const { return frontColor; }
	const Vec3 getBodyColor() const { return bodyColor; }
	const Vec3 getWingColor() const { return wingColor; }
	const Vec3 getWireColor() const { return wireColor; }

private:
	// Movement attributes
	GLdouble maxSpeed;			// Maximum speed
	GLdouble maxForce;			// Maximum steering force
	GLdouble neighRadius;		// Neighborhood radius
	GLdouble separationRadius;	// Separation radius
	GLdouble yaw;				// Facing direction in degrees

	// Weights for behaviors
	GLdouble weightCohesion;	// Weight for cohesion behavior
	GLdouble weightSeparation;	// Weight for separation behavior
	GLdouble weightAlignment;	// Weight for alignment behavior

	// Body colors
	Vec3 frontColor, bodyColor, wingColor, wireColor = Color::Black;

	// Wing animation state
	GLdouble wingAngle;		// Current wing angle
	GLdouble wingAmplitude; // Wing flapping amplitude
	GLdouble wingBaseRate;	// Wing flapping base rate

	// Leader
	ControlledBoid* leaderBoid = nullptr; // Pointer to the controlled boid leader

	// Behavior methods
	void applyBehaviors(const std::vector<Boid*>& neighbors, GLdouble dt);
	void flock(const std::vector<Boid*>& neighbors, GLdouble& rx, GLdouble& ry, GLdouble& rz);
	void separate(const std::vector<Boid*>& neighbors, GLdouble& rx, GLdouble& ry, GLdouble& rz);
	void align(const std::vector<Boid*>& neighbors, GLdouble& rx, GLdouble& ry, GLdouble& rz);
};

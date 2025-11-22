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
	void update(const std::vector<Boid*>& boids, GLfloat deltaTime);
	
	// Draw the boid
	void draw() override;
	void drawShadow();
	void drawBody();

	// Set leader boid
	void setLeader(ControlledBoid* leader) { leaderBoid = leader; }
	
	// Getters for movement attributes
	GLfloat getYaw() const { return yaw; }
	GLfloat getMaxSpeed() const { return maxSpeed; }
	GLfloat getWingAngle() const { return wingAngle; }
	GLfloat getWingAmplitude() const { return wingAmplitude; }
	GLfloat getWingBaseRate() const { return wingBaseRate; }

	// Setters for movement attributes
	void setYaw(GLfloat y) { yaw = y; }
	void setMaxSpeed(GLfloat speed) { maxSpeed = speed; }
	void setWingAngle(GLfloat angle) { wingAngle = angle; }
	void setWingAmplitude(GLfloat amplitude) { wingAmplitude = amplitude; }
	void setWingBaseRate(GLfloat rate) { wingBaseRate = rate; }

	// Set body colors
	void setColors(const Vec3 front, const Vec3 body, const Vec3 wing);

	// Getters for colors
	const Vec3 getFrontColor() const { return frontColor; }
	const Vec3 getBodyColor() const { return bodyColor; }
	const Vec3 getWingColor() const { return wingColor; }
	const Vec3 getWireColor() const { return wireColor; }

private:
	// Movement attributes
	GLfloat yaw;				// Facing direction in degrees
	GLfloat maxSpeed;			// Maximum speed
	GLfloat maxForce;			// Maximum steering force
	GLfloat neighRadius;		// Neighborhood radius
	GLfloat separationRadius;	// Separation radius

	// Weights for behaviors
	GLfloat weightCohesion;	// Weight for cohesion behavior
	GLfloat weightSeparation;	// Weight for separation behavior
	GLfloat weightAlignment;	// Weight for alignment behavior

	// Body colors
	Vec3 frontColor, bodyColor, wingColor, wireColor = Color::Black;

	// Wing animation state
	GLfloat wingAngle;		// Current wing angle
	GLfloat wingAmplitude; // Wing flapping amplitude
	GLfloat wingBaseRate;	// Wing flapping base rate

	// Body dimensions
	GLfloat bodyLength, bodyRadius;
	GLfloat noseLength, noseRadius;
	GLfloat tailLength, tailRadius;
	GLfloat wingSpan, wingChord, wingThickness;

	// Leader
	ControlledBoid* leaderBoid = nullptr; // Pointer to the controlled boid leader

	// Behavior methods
	void applyBehaviors(const std::vector<Boid*>& neighbors, GLfloat dt);
	void flock(const std::vector<Boid*>& neighbors, GLfloat& rx, GLfloat& ry, GLfloat& rz);
	void separate(const std::vector<Boid*>& neighbors, GLfloat& rx, GLfloat& ry, GLfloat& rz);
	void align(const std::vector<Boid*>& neighbors, GLfloat& rx, GLfloat& ry, GLfloat& rz);
	
	// Helper method to draw the boid geometry
	void drawGeometry(bool useColor) const;
};

#include "Object.h"

Object::Object() :
	position(Zero),
	rotation(Zero),
	velocity(Zero),
	size(1.0, 1.0, 1.0) {}

Object::~Object() {}

void Object::setPosition(GLdouble x, GLdouble y, GLdouble z)
{
	position = Vec3(x, y, z);
}

void Object::setPosition(Vec3 pos)
{
	position = pos;
}

void Object::setRotation(GLdouble x, GLdouble y, GLdouble z)
{
	rotation = Vec3(x, y, z);
}

void Object::setRotation(Vec3 rot)
{
	rotation = rot;
}

void Object::setVelocity(GLdouble x, GLdouble y, GLdouble z)
{
	velocity = Vec3(x, y, z);
}

void Object::setVelocity(Vec3 v)
{
	velocity = v;
}

void Object::setSize(GLdouble x, GLdouble y, GLdouble z)
{
	size = Vec3(x, y, z);
}

void Object::setSize(Vec3 s)
{
	size = s;
}

void Object::applyForce(GLdouble fx, GLdouble fy, GLdouble fz)
{
	velocity += Vec3(fx, fy, fz);
}

void Object::applyForce(Vec3 force)
{
	velocity += force;
}

void Object::updatePosition(GLdouble deltaTime)
{
	position += velocity * deltaTime;
}

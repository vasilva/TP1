#include "Object.h"

Object::Object() :
	position(Zero),
	rotation(Zero),
	velocity(Zero),
	size(1.0f, 1.0f, 1.0f),
	hasCollider(true) {}

void Object::setPosition(GLfloat x, GLfloat y, GLfloat z)
{
	position = Vec3(x, y, z);
}

void Object::setPosition(Vec3 pos)
{
	position = pos;
}

void Object::setRotation(GLfloat x, GLfloat y, GLfloat z)
{
	rotation = Vec3(x, y, z);
}

void Object::setRotation(Vec3 rot)
{
	rotation = rot;
}

void Object::setVelocity(GLfloat x, GLfloat y, GLfloat z)
{
	velocity = Vec3(x, y, z);
}

void Object::setVelocity(Vec3 v)
{
	velocity = v;
}

void Object::setSize(GLfloat x, GLfloat y, GLfloat z)
{
	size = Vec3(x, y, z);
}

void Object::setSize(Vec3 s)
{
	size = s;
}

void Object::applyForce(GLfloat fx, GLfloat fy, GLfloat fz)
{
	velocity += Vec3(fx, fy, fz);
}

void Object::applyForce(Vec3 force)
{
	velocity += force;
}

void Object::updatePosition(GLfloat deltaTime)
{
	position += velocity * deltaTime;
}

#include "Box.h"

Box::Box()
{
	body = new cyclone::RigidBody;
	base_mass = 1;
	randomWeight(true);
}

// Renders the box with the correct dimensions/rotation
void Box::render()
{
	GLfloat mat[16];
    body->getGLTransform(mat);
	
    glPushMatrix();
    glMultMatrixf(mat);
    glScalef(halfSize.x*2, halfSize.y*2, halfSize.z*2); // Use the dimensions of the rigid body for the 'mesh'
    glutSolidCube(1.0f);
    glPopMatrix();
}

void Box::reset()
{
	body->setOrientation(1,0,0,0);
    body->setVelocity(0,0,0);
    body->setRotation(0,0,0);
}

void Box::randomWeight(cyclone::Random rdm)
{
	cyclone::real mass = rdm.randomInt(50) + rdm.randomInt(40) + rdm.randomInt(30) + base_mass + rdm.randomInt(100);
	body->setMass(mass);
}

Box::~Box()
{
    delete body;
}
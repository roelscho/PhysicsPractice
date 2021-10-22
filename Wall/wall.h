#include <gl/glut.h>
#include <cyclone/cyclone.h>
#include <cyclone/pfgen.h>
#include "src\app.h"
#include "src\timing.h"
#include <stdio.h>
#include <time.h>

#include <cassert>
#include <iostream>
#include "Box.h"

#define BASE_MASS 3
#define BOX_COUNT 16

class wall : public RigidBodyApplication
{
	Box boxes[BOX_COUNT];
	Box boxProjectile;
	Box floor;
	cyclone::CollisionPlane plane;

	virtual void updateObjects(cyclone::real duration);
	virtual void generateContacts();
    	virtual void update();
	virtual void reset();
	virtual void reset(bool randomWeight);
	virtual void resetProjectile();
    	virtual void key(unsigned char key);

public:
wall();
~wall();	
void createBox(Box *box, cyclone::Vector3 position, cyclone::Vector3 dimensions);
virtual const char* getTitle();
virtual void display();

private:
	cyclone::Random rdm;
	int shot_state;
	clock_t timeShot, current;

	float width;
	float height;
	float depth;
	bool rotate;
	
};

#include <gl/glut.h>
#include <cyclone/cyclone.h>
#include "src\app.h"
#include "src\timing.h"
#include <cyclone/pfgen.h>

#include <stdio.h>
#include <cassert>

#define ROD_COUNT 6
#define SPRING_COUNT 4
#define BASE_MASS 3

/*
A plane is being held up by 4 springlike cables.
The cables are attached to 4 fixed points.
Forces can be applied using the WASD keys.

Use a MassAggregateApplication to create mixture of hard and elastic constraints
*/

class platform : public MassAggregateApplication
{
	cyclone::ParticleRod *rods;
	cyclone::ParticleSpring *spring1;
	cyclone::ParticleSpring *spring2;
	cyclone::ParticleSpring *spring3;
	cyclone::ParticleSpring *spring4;

public:
	platform();
	virtual ~Practicum3();    
	virtual const char* getTitle();
	virtual void display();
	virtual void update();
	virtual void key(unsigned char key);
};

platform::platform():MassAggregateApplication(8), rods(0) //contructor
{
	// Create the first 4 particles that are used as fixed points
	// Create the 4 particles wich resemble the four corners of the plane
	particleArray[0].setPosition(-1.0f,6,1);
	particleArray[1].setPosition(1.0f,6,1);
	particleArray[2].setPosition(1.0f,6,-1);
	particleArray[3].setPosition(-1.0f,6,-1);
	particleArray[4].setPosition(-1.0f,5,1);
	particleArray[5].setPosition(1.0f,5,1);
	particleArray[6].setPosition(1.0f,5,-1);
	particleArray[7].setPosition(-1.0f,5,-1);

	// Add physics: mass, damping and gravity
	for(int i = 4; i < 8; i++)
	{
		particleArray[i].setMass(BASE_MASS); 
		particleArray[i].setDamping(0.5f); 
		particleArray[i].setAcceleration(cyclone::Vector3::GRAVITY);
		particleArray[i].clearAccumulator();
	}
		
	//Create two different springconstants to aplly stiffness
	float springConstant1 = 30.0f;
	float springConstant2 = 15.0f;

	//Create springs to attach the plane to the fixed points. 
	//Assign different spring constants to different sides of the plane.
	spring1 = new cyclone::ParticleSpring(&particleArray[0], springConstant2, 2);
	world.getForceRegistry().add(&particleArray[4], spring1);

	spring2 = new cyclone::ParticleSpring(&particleArray[1], springConstant1, 2);
	world.getForceRegistry().add(&particleArray[5], spring2);

	spring3 = new cyclone::ParticleSpring(&particleArray[2], springConstant1, 2);
	world.getForceRegistry().add(&particleArray[6], spring3);

	spring4 = new cyclone::ParticleSpring(&particleArray[3], springConstant2, 2);
	world.getForceRegistry().add(&particleArray[7], spring4);

	//Attach all the particles and springs to eachother.
	//first the borders of the plane and then two diagonal lines to keep the plane in the right shape.
	rods = new cyclone::ParticleRod[ROD_COUNT];
	float width = 2.0f;
	float depth = 2.0f;
	int r = 0;
	
	rods[r].particle[0] = &particleArray[4];
	rods[r].particle[1] = &particleArray[5];
	rods[r].length = width;
	r++;
	rods[r].particle[0] = &particleArray[5];
	rods[r].particle[1] = &particleArray[6];
	rods[r].length = depth;
	r++;
	rods[r].particle[0] = &particleArray[6];
	rods[r].particle[1] = &particleArray[7];
	rods[r].length = width;
	r++;
	rods[r].particle[0] = &particleArray[7];
	rods[r].particle[1] = &particleArray[4];
	rods[r].length = depth;
	r++;
	rods[r].particle[0] = &particleArray[4];
	rods[r].particle[1] = &particleArray[6];
	rods[r].length = sqrt(width*width + depth*depth);
	r++;
	rods[r].particle[0] = &particleArray[5];
	rods[r].particle[1] = &particleArray[7];
	rods[r].length = sqrt(width*width + depth*depth);
	r++;

	// Add the rods the the ContactGenerator of the world so they are found by the update
	for (unsigned i = 0; i < ROD_COUNT; i++)
        world.getContactGenerators().push_back(&rods[i]);
}

platform::~platform() //destructor
{
	if (rods) delete[] rods;
}

void platform::display()
{
	// Draws the 3D world
	MassAggregateApplication::display();

	//Background color
	glClearColor(0.5f,0.5f,0.5f,1);

	glBegin(GL_LINES);

	// Plane color
	glColor3f(1,0,0);
	// Move through all combinations of lines to draw a plane 
	for(int i = 4; i < 8; i++)
	{
		for(int j = 4; j < 8; j++)
		{
			if(i == j)
			{
				continue;
			}

			const cyclone::Vector3 &p0 = particleArray[i].getPosition();
			const cyclone::Vector3 &p1 = particleArray[j].getPosition();
			glVertex3f(p0.x, p0.y, p0.z);
			glVertex3f(p1.x, p1.y, p1.z);
		}
	}
	
	//Cable color
	glColor3f(0,1,0);
	for(int i = 0; i < 4; i++)
	{
		const cyclone::Vector3 &p0 = particleArray[i].getPosition();
		const cyclone::Vector3 &p1 = particleArray[i+4].getPosition();
		glVertex3f(p0.x, p0.y, p0.z);
		glVertex3f(p1.x, p1.y, p1.z);
	}

	glEnd(); 
}

//Update function
void platform::update()
{
	MassAggregateApplication::update();
}

// Check wich keys are pressed to add forces to the plane.
void platform::key(unsigned char key)
{
	//Velocity vector
	cyclone::Vector3 *velocity = new cyclone::Vector3(0,0,0);
	int acceleration = 4;
	switch(key)
	{
		case 'w':
		velocity = new cyclone::Vector3(0,acceleration,0);
		break;
		case 'a':
		velocity = new cyclone::Vector3(-acceleration,0,0);
		break;
	    	case 's':
		velocity = new cyclone::Vector3(0,-acceleration,0);
		break;
		case 'd':
		velocity = new cyclone::Vector3(acceleration,0,0);
		break;

	    	default:
		MassAggregateApplication::key(key);
	}
	// Add the velocity to each particle
	if(velocity->x == 0 && velocity->y == 0 && velocity->z == 0)
		return;
	for(int i = 0; i < 4; i++)
	{
		particleArray[i+4].setVelocity(velocity->x, velocity->y, velocity->z);
	}
}

const char* platform::getTitle()
{
    return "Platform";
}

Application* getApplication()
{
    return new platform();
}

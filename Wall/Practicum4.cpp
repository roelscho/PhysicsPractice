#include "Practicum4.h"

/*
	The application starts in a sleep state.
	This state can be started or stopped by pressing 'p'.
	Controls:
	'R': Reset simulation
	'N': Reset simulation with new random generated masses
	'+': Increase mass of projectile
	'-': Decrease mass of projectile
	'w': Move projectuile up
	's': Move projectile down
	'd': Rotate projectile clockwise
	'a': Rotate projectile counter-clockwise
	'f': Fire projectile
*/

wall::wall()
:
RigidBodyApplication()
{
	//Create plane
	plane.direction = cyclone::Vector3(0,1,0);
	plane.offset = 0;

	//Size of the boxes
	width = 3.0f;
	height = 3.0f;
	depth = 3.0f;

	//Starting mass of projectile
	boxProjectile.body->setMass(300);

	shot_state = 0; //Set to 1 if projectile is fired

	// Reset projectile position
	reset(true);
	resetProjectile();
}

wall::~wall()
{
	delete boxes;
}

void wall::createBox(Box *box, cyclone::Vector3 position, cyclone::Vector3 dimensions)
{
	box->halfSize = dimensions;			//Set size
    box->body->setPosition(position);	//Set position
    box->body->setOrientation(1,0,0,0); //Set orientation
    box->body->setVelocity(0,0,0);		//Set Velocity
    box->body->setRotation(0,0,0);		//Set rotation
	//Add physics
    cyclone::Matrix3 it;
	it.setBlockInertiaTensor(box->halfSize, box->body->getMass());
    box->body->setInertiaTensor(it);
    box->body->setDamping(0.9f, 0.9f);
    box->body->calculateDerivedData();
    box->calculateInternals();

	//Set sleep state once the boxes are created
	box->body->setCanSleep(true);
	box->body->setAwake(false);
}

void wall::display()
{
	RigidBodyApplication::display();

	//Draw Boxes
    glEnable(GL_NORMALIZE);
	for(int c = 0; c < BOX_COUNT; c++)
	{
		glColor3f(0.5f, 0.5f, 0.5f); 
		boxes[c].render();
	}

	//Draw Projectile
	glColor3f(0.0f, 0.0f, 0.0f);
	boxProjectile.render();

	glEnd();
}

void wall::updateObjects(cyclone::real duration)
{
	//Read current system time and reset projectile after 3 seconds after firing
	current = clock();
	
	if(shot_state == 1 && (current/CLOCKS_PER_SEC - timeShot/CLOCKS_PER_SEC) >= 3)
	{
		resetProjectile();
		shot_state = 0;
	}

	//Create physics for each box
	for(int i = 0; i < BOX_COUNT; i++)
	{
		boxes[i].body->integrate(duration);
		boxes[i].calculateInternals();
	}

	boxProjectile.body->integrate(duration);
	boxProjectile.calculateInternals();
}

void wall::generateContacts()
{
	//Reset the contacts to calculate new collisions
	cData.reset(maxContacts);
    cData.friction = (cyclone::real)0.9;
    cData.restitution = (cyclone::real)0.1;
    cData.tolerance = (cyclone::real)0.1;

	int j_min = 0;
	for(int i = 0; i < BOX_COUNT; i++)
	{
		//Check collision with the floor
		cyclone::CollisionDetector::boxAndHalfSpace(boxes[i], plane, &cData);

		//Check collision with the Projectile
		cyclone::CollisionDetector::boxAndBox(boxProjectile, boxes[i], &cData);
		if (!cData.hasMoreContacts()) continue; 
		
		//Check collisions between the boxes
		for(int j = j_min; j < BOX_COUNT; j++)
		{
			if(i == j) 
				continue;
			if (!cData.hasMoreContacts()) 
				return;

			cyclone::CollisionDetector::boxAndBox(boxes[i], boxes[j], &cData);
		}
		
	}

}

//Reset the boxes with a new random generated mass
void wall::reset(bool randomWeight)
{
	//Get starting box dimensions
	cyclone::Vector3 dimensions = cyclone::Vector3(width/2, height/2, depth/2);
	int boxCount = 0;
	float offset_x = 0;
	
	for(int y = 0; y < 4; y++) // number of boxes in the height
	{
		float y_pos = 0 + y * height;
		int rowCount = 4 ; // Number of boxes in a row
	
		for(int x = 0; x < rowCount; x++)
		{
			float x_pos = -width*2 + x * width + offset_x;
			createBox(&boxes[boxCount], cyclone::Vector3(x_pos, y_pos, -5), dimensions);
			boxes[boxCount].body->setAcceleration(cyclone::Vector3::GRAVITY);

			rdm = cyclone::Random(x+y*y+boxCount + current/CLOCKS_PER_SEC);
			boxes[boxCount].base_mass = rdm.randomInt(100) + 1;
			if(randomWeight)
				boxes[boxCount].randomWeight(rdm);
			boxCount++;
		}
	}
	if(randomWeight == true)
		reset(false);	
}

// resets the Projectile to its original position
void wall::resetProjectile()
{
	createBox(&boxProjectile, cyclone::Vector3(-1, 6, 4), cyclone::Vector3(depth/2, height/2, width/2));
	boxProjectile.body->setAwake(true);
	boxProjectile.body->setVelocity(0,0,0);
	 cyclone::Matrix3 it;
	it.setBlockInertiaTensor(boxProjectile.halfSize, boxProjectile.body->getMass());
    boxProjectile.body->setInertiaTensor(it);
}

// Keypress handling for manipulating the Projectile
void wall::key(unsigned char key)
{
	cyclone::real mass = 0;
	cyclone::Vector3 velocity;
	cyclone::Vector3 position_curr;
	cyclone::Quaternion quat;

	boxProjectile.body->setAwake(true);

	switch(key)
    {
	case 'f':
		if(shot_state != 0) break;
		timeShot = clock();
		// adjust the velocity so it takes the direction in account
		velocity = boxProjectile.body->getTransform().transformDirection(cyclone::Vector3(0,0,-50));
		boxProjectile.body->addVelocity(velocity);
		shot_state = 1;
        break;
	//Increase the mass by 25
	case '+': case '=':
		mass = boxProjectile.body->getMass();
		if(mass <= 0) break;
		boxProjectile.body->setMass(mass + 25);
		std::cout << "projectile mass = " << boxProjectile.body->getMass() << "\n";
		break;
	//Decrease the mass by 25
	case '-': case '_':
		mass = boxProjectile.body->getMass();
		if(!(mass >= 10)) break; // but prevent the mass from going below 0
		boxProjectile.body->setMass(mass - 25);
		std::cout << "projectile mass = " << boxProjectile.body->getMass() << "\n";
		break;
		
	//Rotate clockwise
	case 'd':
		boxProjectile.direction = cyclone::Vector3(0,-0.20,0);
		break;
	//Rotate counter-clockwise
	case 'a':
		boxProjectile.direction = cyclone::Vector3(0,0.20,0);
		break;
	//Move projectile up
	case 'w':
		position_curr = boxProjectile.body->getPosition();
		position_curr.y += 0.25f;
		boxProjectile.body->setPosition(position_curr);
		break;
	//Move projectile down
	case 's':
		position_curr = boxProjectile.body->getPosition();
		position_curr.y -= 0.25f;
		boxProjectile.body->setPosition(position_curr);
		break;
	//Reset boxes with random mass
	case 'n':
		reset(true);
	default:
		break;
	}

	RigidBodyApplication::key(key);
}

//Update function
void wall::update()
{
	boxProjectile.body->addRotation(boxProjectile.direction);
	boxProjectile.direction = cyclone::Vector3(0,0,0);
	RigidBodyApplication::update();
}

//Reset Function
void wall::reset()
{
	reset(false);
}

const char* wall::getTitle()
{
    return "Wall";
}

Application* getApplication()
{
	return new wall();
}
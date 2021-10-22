#include <gl/glut.h>
#include <cyclone/cyclone.h>
#include <cyclone/pfgen.h>

class Box : public cyclone::CollisionBox
{
public:
	Box();
	virtual ~Box();
	void render();
	void randomWeight(cyclone::Random rdm);
	void reset();

public:
	cyclone::Vector3 direction;
	cyclone::Vector3 color;
	cyclone::real base_mass;

};


#include "Rod.h"


using namespace P6;

ParticleContact* Rod::GetContact()
{
	float currLen = CurrentLength();

	std::cout << "a" <<std::endl;
	//if currlen is same as set length skip
	if (currLen == length) {
		return nullptr;
	}

	//create contact
	ParticleContact* ret = new ParticleContact();
	ret->particles[0] = particles[0];
	ret->particles[1] = particles[1];

	//get dir
	MyVector dir = particles[1]->GetPosition() - particles[0]->GetPosition();
	dir = dir.Normalize();

	//if currlen is > the set length
	if (currLen > length) {
		ret->contactNormal = dir;
		//depth must always be pos to interpen
		ret->depth = currLen - length;
	}
	else {

		ret->contactNormal = dir * -1;
		//depth must always be pos to interpen
		ret->depth = length - currLen;
	}

	ret->restitution = restitution;

	return ret;
}

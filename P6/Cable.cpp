#include "Cable.h"

using namespace P6;
ParticleContact* Cable::GetContact()
{
	float currLen = CurrentLength();
	

	//if currlen is same or less as set length skip, similar to the rod but allowing for distance to be shortened instead of constant
	if (currLen <= length) {
		return nullptr;
	}

	//create contact
	ParticleContact* ret = new ParticleContact();
	ret->particles[0] = particles[0];



	//get dir between the anchor point and the particles currentpos
	MyVector dir = anchorPoint - particles[0]->GetPosition();
	dir = dir.Normalize();

	//if currlen is > the set length, less states to check than a rod
	if (currLen > length) {
		ret->contactNormal = dir;
		//depth must always be pos to interpen
		ret->depth = currLen - length;
	}
	
	//resolve as normal
	ret->restitution = restitution;

	return ret;
}

float P6::Cable::CurrentLength()  //length getter override to make use of the anchor point instead of two particle positions
{
	MyVector ret = anchorPoint - particles[0]->GetPosition();
	return ret.Magnitude();
}
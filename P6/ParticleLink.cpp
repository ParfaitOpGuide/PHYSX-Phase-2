#include "ParticleLink.h"

float P6::ParticleLink::CurrentLength()
{
	MyVector ret = particles[0]->GetPosition() - particles[1]->GetPosition();
	return ret.Magnitude();
}

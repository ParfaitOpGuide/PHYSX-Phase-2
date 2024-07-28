#include "ParticleSpring.h"

void P6::ParticleSpring::UpdateForce(Model* particle, float time)
{
	MyVector pos = particle->GetPosition();
	//vector pos -> anchor point
	MyVector force = pos - otherParticle->GetPosition();

	//mag of that vector
	float mag = force.Magnitude();

	// f = -k * (|curr - lrest|)
	float springForce = -springConstant * abs(mag - restLength);

	//direction from particle to anchor
	force = force.Normalize();

	//apply force using that above direction
	force = force * springForce;

	//Add force
	particle->AddForce(force);
}

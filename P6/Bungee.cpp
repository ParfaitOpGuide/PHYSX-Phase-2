#include "Bungee.h"

using namespace P6;

void Bungee::UpdateForce(Model* particle, float time)
{


	MyVector pos = particle->GetPosition();
	//vector pos -> anchor point
	MyVector force = pos - anchorPoint;


	if (force.Magnitude() >= restLength) { // checks if it meets the min required length to start the dampening forces
		//mag of that vector
		float mag = force.Magnitude();

		// f = -k * (|curr - lrest|)
		float bungeeForce = -bungeeConstant * abs(mag - restLength);

		//direction from particle to anchor
		force = force.Normalize();

		//apply force using that above direction
		force = force * bungeeForce;

		//Add force
		particle->AddForce(force);
	} // since it only applies forces at such a specific point compared to a spring, its applied forces are much subtler 
	//and wont kill someone if you threw them off a cliff and saw them bounce back
}

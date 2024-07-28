#include "Chain.h"

using namespace P6;
void Chain::UpdateForce(Model* particle, float time)
{


	MyVector pos = particle->GetPosition();
	//vector pos -> anchor point
	MyVector force = pos - anchorPoint;

	
	if (force.Magnitude() >= maxLength) { // just checks is the istance between the two points is greater r equal to the require length to start force updates
		//mag of that vector
		float mag = force.Magnitude();

		// f = -k * (|curr - lrest|)
		float chainForce = -chainConstant * abs(mag - maxLength);

		//direction from particle to anchor
		force = force.Normalize();

		//apply force using that above direction
		force = force * chainForce;

		//Add force
		particle->AddForce(force);
	} // stiffness and force applied is very harsh and abrupt since chains are usually not made of very elastic materials
}


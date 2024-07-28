#include "ParticleContact.h"

using namespace P6;

void ParticleContact::Resolve(float time) {
	ResolveVelocity(time);
	ResolveInterpenetration(time);
}

float ParticleContact::GetSeparatingSpeed() {
	//Av
	MyVector velocity = particles[0]->Velocity;
	//                       Av - Bv
	if (particles[1]) velocity -= particles[1]->Velocity;
	// (Av-Bv) dot N
	return velocity.ScalarProd(contactNormal);
}

void ParticleContact::ResolveVelocity(float time) {
	float seperatingSpeed = GetSeparatingSpeed();

	if (seperatingSpeed > 0) {
		return; // exit early if obj already moving away from each other
	}
	// SAfter = -c * sS
	float newSS = -restitution * seperatingSpeed;
	//Mag of delta vel
	float deltaSpeed = newSS - seperatingSpeed;
	//total inverse mass
	float totalMass = (float)1 / particles[0]->Mass;
	if (particles[1]) totalMass += (float)1 / particles[1]->Mass;

	//invalid if mass is 0 or less
	if (totalMass <= 0) return;

	//  i = mass * deltaspeed
	float impulse_mag = deltaSpeed / totalMass;
	// get impulse vec based on contactNormal;

	MyVector Impulse = contactNormal * impulse_mag;

	// V = I / M
	MyVector V_a = Impulse * ((float)1 / particles[0]->Mass);
	// Vf = Vi + I/m
	particles[0]->Velocity = particles[0]->Velocity + V_a;

	if (particles[1]) {
		// V = I / M
		MyVector V_b = Impulse * ((float)-1 / particles[1]->Mass);
		// Vf = Vi + I/m
		particles[1]->Velocity = particles[1]->Velocity + V_b;
	}

}

void ParticleContact::ResolveInterpenetration(float time) {
	if (depth <= 0) return;  // skip if not overlapping

	float totalMass = (float)1 / particles[0]->Mass;
	if (particles[1]) totalMass += (float)1 / particles[1]->Mass;

	if (totalMass <= 0) return;

	//how many units to move per total mass
	float totalMoveByMass = depth / totalMass;

	//Get the vector of the total movement involved
	MyVector moveByMass = contactNormal * totalMoveByMass;

	//get the change in position for A
	MyVector P_a = moveByMass * ((float)1 / particles[0]->Mass);
	//translate A
	particles[0]->AddToPosition(P_a);

	if (particles[1]) {
		//get the change in position for b in the opposite dir
		MyVector P_b = moveByMass * (-(float)1 / particles[1]->Mass);
		//translate B
		particles[1]->AddToPosition(P_b);
	}

	depth = 0;
}
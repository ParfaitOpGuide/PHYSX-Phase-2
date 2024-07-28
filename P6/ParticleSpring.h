#pragma once
#include "ForceGenerator.h"
namespace P6 {
	class ParticleSpring : public ForceGenerator
	{
	private:
		Model* otherParticle; //the other particle its attached to
		float springConstant;
		float restLength;

	public:
		ParticleSpring(Model* particle, float _springConst, float _restLen) : otherParticle(particle), springConstant(_springConst), restLength(_restLen) {}

		//override updateforce
		void UpdateForce(Model* particle, float time) override;
	};
}


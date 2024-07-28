#pragma once
#include "../Object/Model.h"
#include "MyVector.h"

namespace P6 {
	class ParticleContact
	{
	public:
		Model* particles[2];
		float restitution; // coefficient of restitution
		MyVector contactNormal;

		float depth;

		void Resolve(float time);

	protected:
		float GetSeparatingSpeed(); 
		void ResolveVelocity(float time);

		void ResolveInterpenetration(float time);
	};

}

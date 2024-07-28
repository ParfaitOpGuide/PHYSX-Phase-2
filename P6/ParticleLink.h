#pragma once
#include "../Object/Model.h"
#include "ParticleContact.h"
namespace P6 {
	class ParticleLink
	{
	public:
		
		Model* particles[2];//particles that are linked
		//returns contact if they violate the terms and conditions of link
		virtual ParticleContact* GetContact() { return nullptr; };

	protected:
		//distance between the 2 particles for interpen
		float CurrentLength();
		
	};
}
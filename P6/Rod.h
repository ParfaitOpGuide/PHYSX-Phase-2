#pragma once
#include "ParticleLink.h"
#include "ParticleContact.h"

namespace P6 {
	class Rod : public ParticleLink
	{
	public:
		float length = 1; //length in m
		float restitution = 0; //rod has no bounce so always has no bounciness

		ParticleContact* GetContact() override;
	};
}


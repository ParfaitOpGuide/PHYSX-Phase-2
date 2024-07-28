#pragma once
#include "ParticleLink.h"
#include "ParticleContact.h"
namespace P6 {
	class Cable : public ParticleLink
	{
	public:
		float length = 1; //length in m
		float restitution = 0;  // i thought you wanted THIS res to be .9 until i realized i was grappling with major brain damage
		MyVector anchorPoint;
		ParticleContact* GetContact() override;
		float CurrentLength();
	};
}


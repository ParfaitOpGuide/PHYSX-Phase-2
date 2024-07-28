#pragma once
#include <vector>
#include "ParticleContact.h"

namespace P6 {
	class ContactResolver
	{
	public:
		//Max number of resolve calls
		unsigned max_iterations;
		//Constructor to assign the max iterations on init
		ContactResolver(unsigned _maxIterations) : max_iterations(_maxIterations){}

		void ResolveContacts(std::vector<ParticleContact*> contacts, float time);

	protected:
		unsigned current_iterations = 0;
	};
	
}

#include "ContactResolver.h"

using namespace P6;

void ContactResolver::ResolveContacts(std::vector<ParticleContact*> contacts, float time) {
	for (unsigned i = 0; i < contacts.size(); i++) {
		contacts[i]->Resolve(time);
	}
}

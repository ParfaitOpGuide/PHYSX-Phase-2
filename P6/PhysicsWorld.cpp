#include "PhysicsWorld.h"


using namespace P6;

void PhysicsWorld::GenerateList(Model* cloneSource) {
	int count;
	std::cout << "Amount of particles: ";
	std::cin >> count;

	std::cout << "Particles do their best now and are preparing. Please watch warmly";

	for (int i = 0; i < count; i++) {
		Model* p = new Model(*cloneSource);
		(*p).Acceleration = P6::MyVector(0, 0, 0);
		(*p).Mass = 1;
		(*p).lifetime = ((rand() % (100 - 10 + 1)) + 10) / 10.0; // generates a float from 1.0 to 10.0 by diving a rand int 100 by float 10.0
		(*p).AddForce(P6::MyVector((rand() % 7000 - 3500), ((rand() % (7400 - 1000 + 1)) + 1000), (rand() % 7000 - 3500)));
		(*p).color = { (rand() % 11) / 10.0 ,(rand() % 11) / 10.0,(rand() % 11) / 10.0 }; // generates a float from 0 to 1 by diving a rand int 10 by float 10.0
		(*p).scale = (rand() % (20 - 4 + 1) + 4);
		this->AddParticle(p);
	}
}

void PhysicsWorld::AddParticle(Model* toAdd) {
	this->Particles.push_back(toAdd);

	forceRegistry.Add(toAdd, &Gravity);
}

void PhysicsWorld::Update(float time, bool paused) {

	if (!paused) {
		UpdateParticleList();

		forceRegistry.UpdateForces(time);

		for (std::list<Model*>::iterator p = Particles.begin(); p != Particles.end(); p++)
		{
			(*p)->Update(time);

			if ((*p)->currTime >= (*p)->lifetime) {
				(*p)->Destroy();

			}
		}

		GenerateContacts();

		if (Contacts.size() > 0)
			contactResolver.ResolveContacts(Contacts, time);
	}
}

void P6::PhysicsWorld::setGravity(float str)
{
	this->Gravity = GravityForceGenerator(MyVector(0, str, 0));
}

void PhysicsWorld::UpdateParticleList() {
	Particles.remove_if(
		[](Model* p) {
			return p->IsDestroyed();
		}
	);
}

void P6::PhysicsWorld::GenerateContacts()
{
	//clear current list
	Contacts.clear();

	this->GetOverlaps();
	//iterate through all links
	for (std::list<ParticleLink*>::iterator i = Links.begin(); i != Links.end(); i++) {
		//get contact for link
		ParticleContact* contact = (*i)->GetContact();
		//if exists push to list
		if (contact != nullptr) {
			Contacts.push_back(contact);
		}
	
	}
}

void PhysicsWorld::GetOverlaps()
{
	for (int i = 0; i < Particles.size() - 1; i++) {
		std::list<Model*>::iterator a = std::next(Particles.begin(), i);
		for (int h = i + 1; h < Particles.size(); h++) {
			std::list<Model*>::iterator b = std::next(Particles.begin(), h);

			MyVector mag2Vector = (*a)->GetPosition() - (*b)->GetPosition();
			float mag2 = mag2Vector.SquareMagnitude();

			float rad = (*a)->radius + (*b)->radius;
			float rad2 = rad * rad;

			//if mag2 == rad2 then touching, if less then overlapping
			if (mag2 <= rad2) {
				MyVector dir = mag2Vector.Normalize();
				float r = rad2 - mag2;
				float depth = sqrt(r);
				float restitution = fmin((*a)->restitution, (*b)->restitution);

				AddContact(*a, *b, restitution, dir, depth);
			}
		}
	}
}

bool PhysicsWorld::AtCenter(Model* p) {
	if (p->z > -1 && p->z < 1)
		return true;
	else return false;
}

void PhysicsWorld::AddContact(Model* p1, Model* p2, float restitution, MyVector contactNormal, float depth) {
	ParticleContact* toAdd = new ParticleContact();

	toAdd->particles[0] = p1;
	toAdd->particles[1] = p2;
	toAdd->restitution = restitution;
	toAdd->contactNormal = contactNormal;
	toAdd->depth = depth;

	Contacts.push_back(toAdd);
}
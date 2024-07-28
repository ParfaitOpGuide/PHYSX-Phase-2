#pragma once
#include "ForceGenerator.h"
namespace P6 {
	class Bungee : public ForceGenerator
	{
	private:
		MyVector anchorPoint; //chin anchor point
		float bungeeConstant; //stiffness
		float restLength; //length at rest
		//also related to when the pull back cuts off to simulate the force dampening of the bungee where its main purpose is to slow you down, not bounce you back

	public:
		Bungee(MyVector pos, float _bungeeConst, float _restLen) : anchorPoint(pos), bungeeConstant(_bungeeConst), restLength(_restLen) {}

		//override updateforce
		void UpdateForce(Model* particle, float time) override;

	};
}


#pragma once
#include "ForceGenerator.h"
namespace P6 {
	class Chain : public ForceGenerator
	{
	private:
		MyVector anchorPoint; //chin anchor point
		float chainConstant; //stiffness, set very high in order to get a hard jerk back when reaching max
		float maxLength; //length at rest, also when the pull back activates

	public:
		Chain(MyVector pos, float _chainConst, float _maxLen) : anchorPoint(pos), chainConstant(_chainConst), maxLength(_maxLen) {}

		//override updateforce
		void UpdateForce(Model* particle, float time) override;

	};
}


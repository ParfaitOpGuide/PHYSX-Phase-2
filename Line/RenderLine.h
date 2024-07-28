#pragma once
#include "../P6/MyVector.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

using namespace P6;

class RenderLine
{
	MyVector P1, P2;
	glm::mat4 projectionMatrix;

public:
	RenderLine(MyVector p1, MyVector p2, glm::mat4 projectionMatrix) : P1(p1), P2(p2), projectionMatrix(projectionMatrix) {}

	void Update(MyVector p1, MyVector p2, glm::mat4 projectionMatrix, int camType);
	void Draw();
};


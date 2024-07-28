#pragma once

#include <glm/glm.hpp>
class MyCamera {
public:
	glm::vec3 cameraPos;
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
	glm::vec3 worldUp;
	//to be able to change view distance through fog fade
	float FogMax;
	float FogMin;
	float cam_theta_mod;
	float cam_phi_mod;
	float radius;
public:
	virtual void processEvents(glm::vec3 modelPos, glm::vec3 objDir) = 0;
};
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <chrono>
#include <conio.h>
//GLM HEADERS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Light/Light.h"
#include "Light/PointLight.h"
#include "Light/DirectionLight.h"

#include "Object/Model.h"


#include "Camera/MyCamera.h"
#include "Camera/OrthoCamera.h"
#include "Camera/FirstPersonCamera.h"
#include "Camera/ThirdPersonCamera.h"

#include "ShaderClasses/Shader.h"
#include "P6/MyVector.h"
#include "P6/PhysicsWorld.h"
#include "P6/ForceGenerator.h"
#include "P6/GravityForceGenerator.h"
#include "P6/DragForceGenerator.h"
#include "P6/ForceRegistry.h"
#include "P6/ParticleSpring.h"
#include "P6/Rod.h"
#include "P6/Chain.h"
#include "P6/Bungee.h"
#include "P6/Cable.h"

#include "Line/RenderLine.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds timestep(16ms);

bool readyToPush = false; //push flag for the  newtons cradle forces

//window size`
float width = 800.0f, height = 800.0f;


int camType = 0;
//create cams
ThirdPersonCamera* ThirdPerson;
//FirstPersonCamera* FirstPerson;
OrthoCamera* Orthographic;
//camera vector to store both cameras
std::vector<MyCamera*> vecCameras;

//lights
PointLight* Point = new PointLight(glm::vec3(-4, 5, 0));
DirectionLight* Direction = new DirectionLight(glm::vec3(0, 20, 0));



//uo direction, usually just 1 in y
glm::vec3 worldUp = glm::vec3(0, 1.0f, 0);

//for mouse movement
bool mouse_start = true;
bool mouse_pause = false;
double mouse_x_last;
double mouse_sens = 0.01;

//bool to close window with a button because im tired of having to press window when my mouse gets locked
bool close = false;

//light state switch
int lightState = 0;

//speed multiplier for movement
float fSpeed = 2.f;
float fOffset = 0.f;

void Key_Callback(GLFWwindow* window, //pointer to window
	int key, //keycode of press
	int scancode, //physical position of the press
	int action,//either press/release
	int mods) //modifier keys
{
	if (action == GLFW_REPEAT || action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_1: //ortho
			camType = 0;
			break;
		case GLFW_KEY_2: //third
			camType = 1;
			break;
		case GLFW_KEY_D:
			vecCameras[camType]->cam_theta_mod += 0.1;
			break;
		case GLFW_KEY_A:
			vecCameras[camType]->cam_theta_mod -= 0.1;
			break;
		case GLFW_KEY_W:
			vecCameras[camType]->cam_phi_mod += 0.03;
			vecCameras[camType]->cam_phi_mod = glm::clamp(vecCameras[camType]->cam_phi_mod, 0.01f, 3.14f);
			break;
		case GLFW_KEY_S:
			vecCameras[camType]->cam_phi_mod -= 0.03;
			vecCameras[camType]->cam_phi_mod = glm::clamp(vecCameras[camType]->cam_phi_mod, 0.01f, 3.14f);
			break;
		case GLFW_KEY_SPACE:
			readyToPush = true;
			break;
		case GLFW_KEY_ESCAPE:
			close = true;
			break;
		}
	}
}

glm::vec3 getAccel(float accel, float pitch, float yaw) {
	return { accel * (sin(pitch) * cos(yaw)), accel * (sin(pitch) * sin(yaw)), accel * cos(pitch) };
}

glm::vec3 getVel(float vel, float pitch, float yaw) {
	return { vel * (sin(pitch) * cos(yaw)), vel * (sin(pitch) * sin(yaw)), vel * cos(pitch) };
}

int main(void)
{
	GLFWwindow* window;


	/* Initialize the library */
	if (!glfwInit())
		return -1;

	float width = 600.0f, height = 600.0f;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Agasen Nathaniel Ryan B.", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	

	//IDENTITY MATRIXES
	glm::mat3 identity_matrix3 = glm::mat3(1.0f);
	glm::mat4 identity_matrix4 = glm::mat4(1.0f);

	//create shader
	Shader* ShaderSkybox = new Shader(window, "Shaders/skyblock.vert", "Shaders/skyblock.frag");
	Shader* Shader1 = new Shader(window, "Shaders/sampleNew.vert", "Shaders/sampleNew.frag");
	Shader* ShaderNormals = new Shader(window, "Shaders/normals.vert", "Shaders/normals.frag");

	std::vector<Shader*> vecShaders;
	vecShaders.push_back(ShaderNormals);
	vecShaders.push_back(Shader1);
	//create objects and pass the path to the things needed for it specifically, obj first then tex, then normal if needed




	ThirdPerson = new ThirdPersonCamera(worldUp, height, width);
	//FirstPerson = new FirstPersonCamera(MainModel, worldUp, height, width);
	Orthographic = new OrthoCamera(worldUp);

	//Load shader file into a strin steam
	std::fstream vertSrc("Shaders/sampleNew.vert");
	std::stringstream vertBuff;
	std::fstream skyboxVertSrc("Shaders/skyblock.vert");
	std::stringstream skyboxVertBuff;

	vertBuff << vertSrc.rdbuf();
	skyboxVertBuff << skyboxVertSrc.rdbuf();

	std::string vertS = vertBuff.str();
	const char* v = vertS.c_str();
	std::string skyboxVertS = skyboxVertBuff.str();
	const char* sky_v = skyboxVertS.c_str();

	//same but for frag
	std::fstream fragSrc("Shaders/sampleNew.frag");
	std::stringstream fragBuff;
	std::fstream skyboxFragSrc("Shaders/skyblock.frag");
	std::stringstream skyboxFragBuff;

	fragBuff << fragSrc.rdbuf();
	skyboxFragBuff << skyboxFragSrc.rdbuf();

	std::string fragS = fragBuff.str();
	const char* f = fragS.c_str();
	std::string skyboxFragS = skyboxFragBuff.str();
	const char* sky_f = skyboxFragS.c_str();



	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	gladLoadGL();

	glViewport(0, 0, width, height);


	//set the callback function to the window
	glfwSetKeyCallback(window, Key_Callback);


	//blend yo mama
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, //source factor
		GL_ONE_MINUS_SRC_ALPHA);

	//camera stuff gets pushed into the vector
	vecCameras.push_back(Orthographic);
	vecCameras.push_back(ThirdPerson);
	//vecCameras.push_back(FirstPerson);
	float cableLen, gap, rad, grav, fX,fY,fZ;//input stuff
	bool hasBeenPushed = false;
	std::cout << "Cable Length: ";
	std::cin >> cableLen;
	std::cout << "Particle Gap: ";
	std::cin >> gap;
	std::cout << "Particle Radius: ";
	std::cin >> rad;
	std::cout << "Gravity Strength: ";
	std::cin >> grav;

	std::cout << "Apply Force" << std::endl;
	std::cout << "X: ";
	std::cin >> fX;
	std::cout << "Y: ";
	std::cin >> fY;
	std::cout << "Z: ";
	std::cin >> fZ;


	P6::PhysicsWorld pWorld = P6::PhysicsWorld();
	pWorld.setGravity(grav);

	std::srand(time(NULL));
	//initialize first model and clone it by passing it to the world
	Model p = Model(window, {  250, 0, 0 }, Shader1->shaderProg, "3D/sphere.obj", "3D/sphere.jpg", rad*1.8, { 1.f,1.f,1.f });
	p.Acceleration = P6::MyVector(0, 0, 0);
	p.Mass = 1;
	p.lifetime = 100;

	Model* p1 = new Model(p);
	p1->Velocity=P6::MyVector(0, 0, 0);
	p1->Mass = 50;
	p1->color = { 0.f, 1.f, 0.f };
	p1->x = gap*2;
	p1->y = 200;
	pWorld.AddParticle(p1);
	

	Model* p2 = new Model(p);
	p2->x = gap;
	p2->y = 200;
	p2->Mass = 50;
	p2->color = { 1.f, 0.f, 0.f };
	p2->Velocity = P6::MyVector(0, 0, 0);
	pWorld.AddParticle(p2);

	Model* p3 = new Model(p);
	p3->x = 0;
	p3->y = 200;
	p3->Mass = 50;
	p3->color = { 0.f, 0.f, 1.f };
	p3->Velocity = P6::MyVector(0, 0, 0);
	pWorld.AddParticle(p3);

	Model* p4 = new Model(p);
	p4->x = -gap;
	p4->y = 200;
	p4->Mass = 50;
	p4->color = { 1.f,  1.f, 0.f };
	p4->Velocity = P6::MyVector(0, 0, 0);
	pWorld.AddParticle(p4);

	Model* p5 = new Model(p);  //this is the one force get applied to
	p5->x = -(gap*2);
	p5->y = 200;
	p5->Mass = 50;
	p5->color = { 0.f, 1.f, 1.f };
	p5->Velocity = P6::MyVector(0, 0, 0);
	pWorld.AddParticle(p5);
	

	//line set up
	RenderLine line1(P6::MyVector(300, 200, 0), p1->GetPosition(), glm::mat4(1.f));
	RenderLine line2(P6::MyVector(150, 200, 0), p2->GetPosition(), glm::mat4(1.f));
	RenderLine line3(P6::MyVector(0, 200, 0), p3->GetPosition(), glm::mat4(1.f));
	RenderLine line4(P6::MyVector(-150, 200, 0), p4->GetPosition(), glm::mat4(1.f));
	RenderLine line5(P6::MyVector(-300, 200, 0), p5->GetPosition(), glm::mat4(1.f));

	//create all the cables and set up which particle and which anchor point theyll be set to
	P6::Cable* c1 = new P6::Cable();
	c1->particles[0] = p1;
	c1->length = cableLen;
	c1->anchorPoint = P6::MyVector(gap*2, 200, 0);
	pWorld.Links.push_back(c1);

	P6::Cable* c2 = new P6::Cable();
	c2->particles[0] = p2;
	c2->length = cableLen;
	c2->anchorPoint = P6::MyVector(gap, 200, 0);
	pWorld.Links.push_back(c2);

	P6::Cable* c3 = new P6::Cable();
	c3->particles[0] = p3;
	c3->length = cableLen;
	c3->anchorPoint = P6::MyVector(0, 200, 0);
	pWorld.Links.push_back(c3);

	P6::Cable* c4 = new P6::Cable();
	c4->particles[0] = p4;
	c4->length = cableLen;
	c4->anchorPoint = P6::MyVector(-gap, 200, 0);
	pWorld.Links.push_back(c4);

	P6::Cable* c5 = new P6::Cable();
	c5->particles[0] = p5;
	c5->length = cableLen;
	c5->anchorPoint = P6::MyVector(-(gap*2), 200, 0);
	pWorld.Links.push_back(c5);



	//clock and var
	using clock = std::chrono::high_resolution_clock;
	auto curr_time = clock::now();
	auto prev_time = curr_time;
	std::chrono::nanoseconds curr_ns(0);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window) && !close)
	{
		//if the space is pressed and it hasnt been pushed yet
		if (readyToPush && !hasBeenPushed) {
			p5->AddForce(P6::MyVector(fX, fY, fZ));
			hasBeenPushed = true;
		}

		//get current time
		curr_time = clock::now();
		//duration between this and last
		auto dur = std::chrono::duration_cast<std::chrono::nanoseconds> (curr_time - prev_time);
		//prev to current for next iteration
		prev_time = curr_time;

		curr_ns += dur;

		if (curr_ns >= timestep) {


			//nano to millie
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);
			//reset
			curr_ns -= curr_ns;

			pWorld.Update((float)ms.count() / 1000, false);
	
		}
		//end clock

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//processes events depending on camtype

		vecCameras[camType]->processEvents({ 0,0,0 }, { 0,0,0 });
		//update the lines every frame

		line1.Update(P6::MyVector(gap*2, 200, 0), p1->GetPosition(), vecCameras[camType]->projectionMatrix, camType);
		line2.Update(P6::MyVector(gap, 200, 0), p2->GetPosition(), vecCameras[camType]->projectionMatrix, camType);
		line3.Update(P6::MyVector(0, 200, 0), p3->GetPosition(), vecCameras[camType]->projectionMatrix, camType);
		line4.Update(P6::MyVector(-gap, 200, 0), p4->GetPosition(), vecCameras[camType]->projectionMatrix, camType);
		line5.Update(P6::MyVector(-gap*2, 200, 0), p5->GetPosition(), vecCameras[camType]->projectionMatrix, camType);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		int shaderCount;
		//pass all necessary drawing stuff to the objects draw, call all objects draw in vector
		for (auto i : pWorld.Particles) {


			//std::cout << shaderCount << std::endl;
			i->processEvents(identity_matrix4,
				ShaderNormals->shaderProg,
				vecCameras[camType],
				Point, Direction);
		}

		
		line1.Draw();
		line2.Draw();
		line3.Draw();
		line4.Draw();
		line5.Draw();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}


	glfwTerminate();
	std::cout << "\nPress any key to end the program: ";
	_getch();

	return 0;
}
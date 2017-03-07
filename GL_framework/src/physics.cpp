#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include <vector>
#include <iostream>

//Boolean variables allow to show/hide the primitives
bool renderSphere = false;
bool renderCapsule = false;
bool renderParticles = true;
bool show_test_window = true;

namespace Sphere {
	extern void setupSphere(glm::vec3 pos = glm::vec3(0.f, 1.f, 0.f), float radius = 1.f);
	extern void cleanupSphere();
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
	extern void drawSphere();
}
namespace Capsule {
	extern void setupCapsule(glm::vec3 posA = glm::vec3(-3.f, 2.f, -2.f), glm::vec3 posB = glm::vec3(-4.f, 2.f, 2.f), float radius = 1.f);
	extern void cleanupCapsule();
	extern void updateCapsule(glm::vec3 posA, glm::vec3 posB, float radius = 1.f);
	extern void drawCapsule();
}
namespace LilSpheres {
	extern const int maxParticles;
	extern void setupParticles(int numTotalParticles, float radius = 0.05f);
	extern void cleanupParticles();
	extern void updateParticles(int startIdx, int count, float* array_data);
	extern void drawParticles(int startIdx, int count);
}

void setupPrims() {
	Sphere::setupSphere();
	Capsule::setupCapsule();

	//TODO
	//You define how many particles will be in the simulation (maxParticles number in render.cpp is defined to SHRT_MAX, 
	//	you can change it if you want, but be aware of troubled outcomes, 
	//	like having to create multiple buffers because of interger overflow...)
	//Link the parameter of setupParticles to the max number of particles in the physics simulation you want to have
	LilSpheres::setupParticles(LilSpheres::maxParticles);
	//

	//TODO
	//updateParticles is the function you can use to update the position of the particles (directly from the physics code)
	//The access is contiguous from an start idx to idx+count particles. You may need to do multiple calls.
	//Called here as an example to initialize to random values all particles inside the box. This code can be removed.
	/*float *partVerts = new float[LilSpheres::maxParticles * 3];
	for (int i = 0; i < LilSpheres::maxParticles; ++i) {
		partVerts[i * 3 + 0] = ((float)rand() / RAND_MAX) * 10.f - 5.f;
		partVerts[i * 3 + 1] = ((float)rand() / RAND_MAX) * 10.f;
		partVerts[i * 3 + 2] = ((float)rand() / RAND_MAX) * 10.f - 5.f;
	}
	LilSpheres::updateParticles(0, LilSpheres::maxParticles, partVerts);
	delete[] partVerts;*/
	//
}
void cleanupPrims() {
	Sphere::cleanupSphere();
	Capsule::cleanupCapsule();
	LilSpheres::cleanupParticles();
}
void renderPrims() {
	if (renderSphere)
		Sphere::drawSphere();
	if (renderCapsule)
		Capsule::drawCapsule();

	//TODO drawParticles can only draw a contiguous amount of particles in its array from start idx to idx+count
	//Depending the alive particles that have to be rendered, you may need to do multiple calls for this function
	if (renderParticles)
		LilSpheres::drawParticles(0, LilSpheres::maxParticles);
	//
}


struct Particle {
	glm::vec3 pos;
	glm::vec3 speed;
	float lifeEx; // inizialitzar a un valor (ex. 1 segon).
};

float gravity = 9.8f;

Particle *partArray;
float *vertexArray;

void generateNewParticle(bool mode) {
	//calcular la generacio de particules, es a dir, la posicio x,y,z de la particula
	//true es cascada, false es font

	//algoritme cascada
	if (mode) {

	}
	//algoritme de font desde un punt
	if (!mode) {

	}

}

void updateParticleArray() {
	for (int i = 0; i < LilSpheres::maxParticles; ++i) {
		vertexArray[i * 3 + 0] = partArray[i].pos[0];
		vertexArray[i * 3 + 1] = partArray[i].pos[1];
		vertexArray[i * 3 + 2] = partArray[i].pos[2];
	}
	LilSpheres::updateParticles(0, LilSpheres::maxParticles, vertexArray);
}

void moveParticle(int index) {
//TO DO
	//actualitzar velocitat
	partArray[index].speed.x = ((float)rand() / RAND_MAX) * 0.5f - 0.5f;//((float)rand() / RAND_MAX) * 0.5f;
	partArray[index].speed.y = ((float)rand() / RAND_MAX) * 0.5f;//((float)rand() / RAND_MAX) * 0.5f;
	partArray[index].speed.z = ((float)rand() / RAND_MAX) * 0.5f - 0.5f;//((float)rand() / RAND_MAX) * 0.5f;
	//actualitzar posicio
	if (partArray[index].speed.x != 0 && partArray[index].speed.y != 0 && partArray[index].speed.z != 0) {
		partArray[index].pos.x += partArray[index].speed.x;
		partArray[index].pos.y += partArray[index].speed.y;
		partArray[index].pos.z += partArray[index].speed.z;
	}
}

void checkWallCollision(int index) {
	//TO DO
	//if collides, update new position and velocity
}

void PhysicsInit() {	
	//inizialitzar particules en el vector partArray;

	partArray = new Particle[LilSpheres::maxParticles];
	vertexArray = new float[LilSpheres::maxParticles*3];

	for (int i = 0; i < LilSpheres::maxParticles; ++i) {
		partArray[i].pos.x = ((float)rand() / RAND_MAX) * 10.f - 5.f;
		partArray[i].pos.y = ((float)rand() / RAND_MAX) * 10.f;
		partArray[i].pos.z = ((float)rand() / RAND_MAX) * 10.f - 5.f;
	}
}


void PhysicsUpdate(float dt){
	
	for (int i = 0; i < LilSpheres::maxParticles; ++i) {
		//mirar si moren
		//generar noves particules

		moveParticle(i);
		//checkWallCollision(i); //recalcular nova posicio
		//colisio amb esfera i capsula


	}
	
	


	updateParticleArray(); //al final de tot
}
void PhysicsCleanup() {
	
	delete[] partArray;
	delete[]vertexArray;
	
}

void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		//TODO
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}	 
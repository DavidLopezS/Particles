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

int youngestPart, oldestPart;



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
	if (renderParticles) {
		if (oldestPart <= youngestPart) {
			LilSpheres::drawParticles(oldestPart, youngestPart + 1 - oldestPart);
		}
		else {
			LilSpheres::drawParticles(0, youngestPart + 1);
			LilSpheres::drawParticles(oldestPart, LilSpheres::maxParticles - oldestPart);
		}
	}

	//
}


struct Particle {
	glm::vec3 pos;
	glm::vec3 prePos;
	glm::vec3 speed;
	glm::vec3 dir;
	float lifeTime;
	Particle() {
		dir.x = dir.y = dir.z = 1;
	}
};

float particlesLifeTime = 3.0f;
int particleGenerationRate = 50;
float gravity = 9.8f;
int isVerletMode = false; //canvia de Euler a Verlet--> false = Euler    true = Verlet.
int numPart = 0; //numero total de particules en cada frame

Particle *partArray;
float *vertexArray;

void particleGenerator() {
	for (int i = 0; i < particleGenerationRate; ++i) {
		if (numPart < LilSpheres::maxParticles) {
			youngestPart = (youngestPart + 1) % LilSpheres::maxParticles;
#pragma region
			//pos inicial
			/*partArray[youngestPart].pos.x = ((float)rand() / RAND_MAX) * 10.f - 5.f;
			partArray[youngestPart].pos.y = ((float)rand() / RAND_MAX) * 10.f;
			partArray[youngestPart].pos.z = ((float)rand() / RAND_MAX) * 10.f - 5.f;*/

			partArray[youngestPart].pos.x = -2;
			partArray[youngestPart].pos.y = 5;
			partArray[youngestPart].pos.z = 0;

			//vel inicial
			partArray[youngestPart].speed.x = ((float)rand() / RAND_MAX) * 5 + 3;
			partArray[youngestPart].speed.y = ((float)rand() / RAND_MAX) * 10 + 5;
			partArray[youngestPart].speed.z = ((float)rand() / RAND_MAX) * 6.f - 3.f;

			partArray[youngestPart].lifeTime = particlesLifeTime;
#pragma endregion SPAWN POSITION
			++numPart;
		}
	}

	////true es cascada, false es font

	////algoritme cascada
	//if (mode) {

	//}
	////algoritme de font desde un punt
	//if (!mode) {

	//}

}
void particleKiller() {
	while (numPart > 0 && partArray[oldestPart].lifeTime <= 0) {
		oldestPart = (oldestPart + 1) % LilSpheres::maxParticles;
		--numPart;
	}
}

void transformParticleArrayToFloatArray() {
	for (int i = 0; i < LilSpheres::maxParticles; ++i) {
		vertexArray[i * 3 + 0] = partArray[i].pos[0];
		vertexArray[i * 3 + 1] = partArray[i].pos[1];
		vertexArray[i * 3 + 2] = partArray[i].pos[2];
	}
}



void moveParticle(int index, float time) {
//TO DO

	//EULER SOLVER (elastic)
	if (!isVerletMode) {//S'utilitza la variable global EuVer
		//actualitzar velocitat
		partArray[index].speed.x = partArray[index].speed.x; //la mateixa, no hi ha fregament
		partArray[index].speed.y = partArray[index].speed.y - gravity*time; //te gravetat
		partArray[index].speed.z = partArray[index].speed.z; //la mateixa, no hi ha fregament
		//actualitzar posicio
		partArray[index].pos.x = partArray[index].pos.x + partArray[index].speed.x * time /** partArray[index].dir.x*/;
		partArray[index].pos.y = partArray[index].pos.y + partArray[index].speed.y * time;
		partArray[index].pos.z = partArray[index].pos.z + partArray[index].speed.z * time /** partArray[index].dir.z*/;
	}
	//VERLET SOLVER
	if (isVerletMode) {//S'utilitza la variable global EuVer
		
		if (partArray[index].prePos.x == 0 && partArray[index].prePos.y == 0 && partArray[index].prePos.z == 0) {
			partArray[index].prePos.x = partArray[index].pos.x;
			partArray[index].prePos.y = partArray[index].pos.y;
			partArray[index].prePos.z = partArray[index].pos.z;
		}

		//actualitzar la posició
		partArray[index].pos.x = partArray[index].pos.x + (partArray[index].pos.x - partArray[index].prePos.x)+0 * (time * time);
		partArray[index].pos.y = partArray[index].pos.y + (partArray[index].pos.y - partArray[index].prePos.y)-gravity * (time * time);
		partArray[index].pos.z = partArray[index].pos.z + (partArray[index].pos.z - partArray[index].prePos.z)+0 * (time * time);

		partArray[index].prePos.x = partArray[index].pos.x;
		partArray[index].prePos.y = partArray[index].pos.y;
		partArray[index].prePos.z = partArray[index].pos.z;

		//std::cout << "Prev pos: " << temp[0] << std::endl;
		//std::cout << "Next pos: " << partArray[index].pos.x << std::endl;
	}
}

void checkWallCollision(int index, float time) {
	//TO DO
	//if collides, update new position and velocity
	if (partArray[index].pos.y <= 0.f) {//Suelo
		//partArray[index].pos.y -= partArray[index].speed.y * time;
		//partArray[index].pos.y = 
	}
	else if (partArray[index].pos.y >= 10.f) {
		//partArray[index].pos.y -= partArray[index].speed.y *time;
	}
	if (partArray[index].pos.x <= -5.f) {//Paret esquerra
		//partArray[index].pos.x -= partArray[index].speed.x * time;
	}
	else if (partArray[index].pos.x >= 5.f) {//Paret dreta
		//partArray[index].pos.x -= partArray[index].speed.x * time;

	}
	if (partArray[index].pos.z <= -5.f) {//Paret fons
		//partArray[index].pos.z -= partArray[index].speed.z * time;
	}
	else if (partArray[index].pos.z >= 5.f) {//Paret front
		/*partArray[index].pos.z -= partArray[index].speed.z * time;*/
	}
}

void PhysicsInit() {	
	//inizialitzar particules en el vector partArray;

	partArray = new Particle[LilSpheres::maxParticles];
	vertexArray = new float[LilSpheres::maxParticles*3];

	youngestPart = -1;
	oldestPart = 0;
	

}


void PhysicsUpdate(float dt){
	particleKiller(); //mata les particules que pasen el temps de vida
	particleGenerator(); //generar noves particules
	if (numPart > 0) {
		if (oldestPart <= youngestPart) {
			for (int i = oldestPart; i <= youngestPart; ++i) {
				moveParticle(i, dt);
				//collision(i, dt); //recalcular nova posicio
				//colisio amb esfera i capsula

				partArray[i].lifeTime -= dt; //actualitzem temps de vida
			}
			
		}
		else { //oldestPart > youngestPart
			for (int j = 0; j <= youngestPart; ++j) {
				moveParticle(j, dt);
				//collision
				partArray[j].lifeTime -= dt;
			}
			for (int k = oldestPart; k < LilSpheres::maxParticles; ++k) {
				moveParticle(k, dt);
				//collision
				partArray[k].lifeTime -= dt;
			}
			
		}
		transformParticleArrayToFloatArray();
		LilSpheres::updateParticles(0, LilSpheres::maxParticles, vertexArray);
	}
}
void PhysicsCleanup() {
	
	delete[] partArray;
	delete[]vertexArray;
	
}


void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
		ImGui::Combo("SOLVER", &isVerletMode, "Euler\0Verlet\0");
		ImGui::DragFloat("Particles Life Time", &particlesLifeTime, 0.1f);
		ImGui::DragInt("Particles Generation Rate", &particleGenerationRate, 1);
		ImGui::DragFloat("Gravity", &gravity, 0.1f);
		//TODO
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);

	}
}	 
#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include <vector>
#include <iostream>

//Boolean variables allow to show/hide the primitives
bool renderSphere = true;
bool renderCapsule = false;
bool renderParticles = true;
bool show_test_window = true;

glm::vec3 spherePos(0.f, 3.f, 0.f);
float sphereRadius = 1.f;

namespace Sphere {
	extern void setupSphere(glm::vec3 pos = spherePos, float radius = sphereRadius);
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

//variables NO modificables al GUI
int youngestPart = -1, oldestPart = 0;
int numPart = 0; //numero total de particules en cada frame

//variables modificables al GUI
float particlesLifeTime = 1.0f;
int particleGenerationRate = 100;
float gravity = 9.8f;
int isVerletMode = false; //canvia de Euler a Verlet--> false = Euler    true = Verlet.
int isCascadeMode = false;

bool useElasticity = false;
bool useFriction = false;
float cE; //elastic coeficient

float elastic = 0.5f;
float friction = 0.5f;

//other variables
int lastSolver = isVerletMode;
Particle *partArray;
float *vertexArray;

void setupPrims() {
	Sphere::setupSphere();
	Capsule::setupCapsule();
	LilSpheres::setupParticles(LilSpheres::maxParticles);
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
		if (numPart > 0) {
			if (oldestPart <= youngestPart) {
				LilSpheres::drawParticles(oldestPart, youngestPart + 1 - oldestPart);
			}
			else {
				LilSpheres::drawParticles(0, youngestPart + 1);
				LilSpheres::drawParticles(oldestPart, LilSpheres::maxParticles - oldestPart);
			}
		}
	}

	//
}

#pragma region
float fountainPos[3] = { 0.5f, 0.5f, 0.5f };
float fountainVelIntensity[3] = { 0,7.5,0 };
float fountainVelRange[3] = { 5,2,5 };

float cascadePos[3] = { 1,1,0.2f };
float cascadeVelZ;
float cascadeVelZRange;
#pragma endregion emitter variables

void particleGenerator(float time) {
	for (int i = 0; i < particleGenerationRate; ++i) {
		if (numPart < LilSpheres::maxParticles) {
			youngestPart = (youngestPart + 1) % LilSpheres::maxParticles;
			if (!isCascadeMode) { //Fountain
				//pos inicial
				partArray[youngestPart].pos.x = fountainPos[0]*8 -4;
				partArray[youngestPart].pos.y = fountainPos[1] * 8 +1;
				partArray[youngestPart].pos.z = fountainPos[2] * 8 - 4;
				//vel inicial
				partArray[youngestPart].speed.x = ((float)rand() / RAND_MAX) * fountainVelRange[0] + fountainVelIntensity[0] - (fountainVelRange[0]/2);
				partArray[youngestPart].speed.y = ((float)rand() / RAND_MAX) * fountainVelRange[1] + fountainVelIntensity[1]- (fountainVelRange[1]/2);
				partArray[youngestPart].speed.z = ((float)rand() / RAND_MAX) * fountainVelRange[2] + fountainVelIntensity[2] - (fountainVelRange[2]/2);
				//prePos
				partArray[youngestPart].prePos.x = partArray[youngestPart].pos.x - partArray[youngestPart].speed.x * time;
				partArray[youngestPart].prePos.y = partArray[youngestPart].pos.y - partArray[youngestPart].speed.y * time;
				partArray[youngestPart].prePos.z = partArray[youngestPart].pos.z - partArray[youngestPart].speed.z * time;
			}
			else if (isCascadeMode) { //Cascade				
				//pos inicial
				partArray[youngestPart].pos.x = ((float)rand() / RAND_MAX) * cascadePos[0]*10 -cascadePos[0]*10 / 2;
				partArray[youngestPart].pos.y = cascadePos[1]*9 +0.5;
				partArray[youngestPart].pos.z = cascadePos[2]*9 -4.5;

				//vel inicial
				partArray[youngestPart].speed.x = 0;
				partArray[youngestPart].speed.y = ((float)rand() / RAND_MAX) * 5 - 10;
				partArray[youngestPart].speed.z = ((float)rand() / RAND_MAX) * cascadeVelZRange + cascadeVelZ - (cascadeVelZRange / 2);

				partArray[youngestPart].prePos.x = partArray[youngestPart].pos.x - partArray[youngestPart].speed.x * time;
				partArray[youngestPart].prePos.y = partArray[youngestPart].pos.y - partArray[youngestPart].speed.y * time;
				partArray[youngestPart].prePos.z = partArray[youngestPart].pos.z - partArray[youngestPart].speed.z * time;
			}
			
			partArray[youngestPart].lifeTime = particlesLifeTime;
			++numPart;
		}
	}
}

void particleKiller() {
	while (numPart > 0 && partArray[oldestPart].lifeTime <= 0) {
		oldestPart = (oldestPart + 1) % LilSpheres::maxParticles;
		--numPart;
	}
}


glm::vec3 tempParticlePos;
void moveParticle(int index, float time) {
//TO DO

	//EULER SOLVER
	if (!isVerletMode) {
		//actualitzar velocitat
		partArray[index].speed.x = partArray[index].speed.x;
		partArray[index].speed.y = partArray[index].speed.y - gravity*time; //te gravetat
		partArray[index].speed.z = partArray[index].speed.z;
		//actualitzar posicio
		partArray[index].pos.x = partArray[index].pos.x + partArray[index].speed.x * time;
		partArray[index].pos.y = partArray[index].pos.y + partArray[index].speed.y * time;
		partArray[index].pos.z = partArray[index].pos.z + partArray[index].speed.z * time;
	}
	//VERLET SOLVER
	if (isVerletMode) {
		//actualitzar la posició
		tempParticlePos.x = partArray[index].pos.x;
		tempParticlePos.y = partArray[index].pos.y;
		tempParticlePos.z = partArray[index].pos.z;

		partArray[index].pos.x = partArray[index].pos.x + (partArray[index].pos.x - partArray[index].prePos.x);
		partArray[index].pos.y = partArray[index].pos.y + (partArray[index].pos.y - partArray[index].prePos.y) - gravity * (time * time);
		partArray[index].pos.z = partArray[index].pos.z + (partArray[index].pos.z - partArray[index].prePos.z);

		partArray[index].prePos.x = tempParticlePos.x;
		partArray[index].prePos.y = tempParticlePos.y;
		partArray[index].prePos.z = tempParticlePos.z;

		partArray[index].speed.x = (partArray[index].pos.x + partArray[index].prePos.x) / time;
		partArray[index].speed.y = (partArray[index].pos.y + partArray[index].prePos.y) / time;
		partArray[index].speed.z = (partArray[index].pos.z + partArray[index].prePos.z) / time;

	}
}

void collidePlane(int index, int A, int B, int C, int d) { //el bo
	glm::vec3 normal = { A, B, C }; //Normal del pla
	float dotProdAct = glm::dot(normal, partArray[index].pos);
	float dotProdPrev = glm::dot(normal, partArray[index].prePos);
	float dotProdSpeed = glm::dot(normal, partArray[index].speed);
	float checkColl = (dotProdAct + d)*(dotProdPrev + d);


	if (checkColl <= 0) {
		if (useElasticity) cE = elastic;
		else cE = 1;
		partArray[index].pos = partArray[index].pos - (1 + cE) * (dotProdAct + d) * normal;
		partArray[index].speed = partArray[index].speed - (1 + cE) * dotProdSpeed * normal;
		if (isVerletMode)
			partArray[index].prePos = partArray[index].prePos - (1 + cE) * (dotProdPrev + d) * normal;
		if (useFriction) {
			glm::vec3 velocityNormal = glm::dot(normal, partArray[index].speed) * normal;
			glm::vec3 velocityTan = partArray[index].speed - velocityNormal;
			partArray[index].speed = partArray[index].speed - friction * velocityTan;
		}
	}
}

void boxCollision(int index) {
	collidePlane(index, 0, 1, 0, 0);//Ground
	collidePlane(index, 0, -1, 0, 10);//Top
	collidePlane(index, 1, 0, 0, 5);//Left Wall
	collidePlane(index, -1, 0, 0, 5);//Right Wall
	collidePlane(index, 0, 0, 1, 5);//Depht Wall
	collidePlane(index, 0, 0, -1, 5);//Front Wall
}

float a, b, c, resPos, resNeg, res, x, y, z; //variables for collide sphere
void collideSphere(int index) {
	

	a = (partArray[index].pos.x - partArray[index].prePos.x) * (partArray[index].pos.x - partArray[index].prePos.x) +
		(partArray[index].pos.y - partArray[index].prePos.y) * (partArray[index].pos.y - partArray[index].prePos.y) +
		(partArray[index].pos.z - partArray[index].prePos.z) * (partArray[index].pos.z - partArray[index].prePos.z);
	b = 2 * ((partArray[index].pos.x - partArray[index].prePos.x) * (partArray[index].prePos.x - spherePos.x) +
		(partArray[index].pos.y - partArray[index].prePos.y) * (partArray[index].prePos.y - spherePos.y) +
		(partArray[index].pos.z - partArray[index].prePos.z) * (partArray[index].prePos.z - spherePos.z));
	c = spherePos.x * spherePos.x + spherePos.y * spherePos.y + spherePos.z * spherePos.z + partArray[index].prePos.x *
		partArray[index].prePos.x + partArray[index].prePos.y * partArray[index].prePos.y + partArray[index].prePos.z *
		partArray[index].prePos.z - 2 * (spherePos.x * partArray[index].prePos.x + spherePos.y * partArray[index].prePos.y +
			spherePos.z * partArray[index].prePos.z) - sphereRadius;

	if (b * b - 4 * a * c >= 0) {
		glm::vec3 auxil, colis;
		resPos = (-b + glm::sqrt(b*b - 4 * a * c)) / (2 * a);
		resNeg = (-b - glm::sqrt(b*b - 4 * a * c)) / (2 * a);
		x = partArray[index].pos.x - partArray[index].prePos.x;
		y = partArray[index].pos.y - partArray[index].prePos.y;
		z = partArray[index].pos.z - partArray[index].prePos.z;
		glm::vec3 coli1 = { partArray[index].prePos.x + x * resPos, partArray[index].prePos.y + y * resPos, partArray[index].prePos.z + z * resPos };
		glm::vec3 coli2 = { partArray[index].prePos.x + x * resNeg, partArray[index].prePos.y + y * resNeg, partArray[index].prePos.z + z * resNeg };
		if (glm::distance(partArray[index].pos, coli1) <= glm::distance(partArray[index].pos, coli2)) {
			res = resPos;
			colis = coli1;
		}
		else {
			res = resNeg;
			colis = coli2;
		}

		glm::vec3 colisNormal = glm::normalize(colis - spherePos);
		float d = colisNormal.x * colis.x + colisNormal.y * colis.y + colisNormal.z * colis.z;
		d = -d;

		float actAux = glm::dot(colisNormal, partArray[index].pos);
		float prevAux = glm::dot(colisNormal, partArray[index].prePos);
		float dotProdSpeed = glm::dot(colisNormal, partArray[index].speed);
		float checkCol = (actAux + d) * (prevAux + d);
		if (checkCol <= 0) {
			partArray[index].pos = partArray[index].pos - 2 * (actAux + d) * colisNormal;
			partArray[index].speed = partArray[index].speed - 2 * dotProdSpeed * colisNormal;
		}
	}
}






void transformParticleArrayToFloatArray() {
	for (int i = 0; i < LilSpheres::maxParticles; ++i) {
		vertexArray[i * 3 + 0] = partArray[i].pos[0];
		vertexArray[i * 3 + 1] = partArray[i].pos[1];
		vertexArray[i * 3 + 2] = partArray[i].pos[2];
	}
}

void PhysicsInit() {	
	//inizialitzar particules en el vector partArray;
	partArray = new Particle[LilSpheres::maxParticles];
	vertexArray = new float[LilSpheres::maxParticles*3];
}

void PhysicsUpdate(float dt){
	if (lastSolver != isVerletMode) { //reset system if user changes solver method
		youngestPart = -1;
		oldestPart = 0;
		numPart = 0;
		lastSolver = isVerletMode;
	}
	particleKiller(); //mata les particules que pasen el temps de vida
	particleGenerator(dt); //generar noves particules
	if (numPart > 0) {
		if (oldestPart <= youngestPart) {
			for (int i = oldestPart; i <= youngestPart; ++i) {
				moveParticle(i, dt);
				boxCollision(i);
				collideSphere(i);
				//colisio amb esfera i capsula
				
				partArray[i].lifeTime -= dt; //actualitzem temps de vida
			}
			
		}
		else { //oldestPart > youngestPart
			for (int j = 0; j <= youngestPart; ++j) {
				moveParticle(j, dt);
				//collision
				boxCollision(j);
				collideSphere(j);
				partArray[j].lifeTime -= dt;
			}
			for (int k = oldestPart; k < LilSpheres::maxParticles; ++k) {
				moveParticle(k, dt);
				//collision
				boxCollision(k);
				collideSphere(k);
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
		ImGui::Text("Particles %i/%i",numPart, LilSpheres::maxParticles);
		ImGui::Combo("Solver", &isVerletMode, "Euler\0Verlet\0");

		ImGui::DragInt("Particle Emission Rate", &particleGenerationRate, 1, 0, 1000);
		ImGui::DragFloat("Particle Life Expectancy", &particlesLifeTime, 0.05f, 0.1f, 15.f, "%.2f");
		ImGui::DragFloat("Gravity", &gravity, 0.1f);

		ImGui::Checkbox("Elastic ", &useElasticity); ImGui::SameLine();
		ImGui::DragFloat("Coefficient value", &elastic, 0.01f, 0.f, 1.f, "%.2f");
		ImGui::Checkbox("Friction", &useFriction); ImGui::SameLine();
		ImGui::DragFloat("Coefficient value", &friction, 0.01f, 0.f, 1.f, "%.2f");

		ImGui::Spacing();
		
		if (ImGui::TreeNode("Emitter")){
			ImGui::Combo("Emitter type", &isCascadeMode, "Fountain\0Cascade\0");
			if (!isCascadeMode){
				ImGui::SliderFloat3("Spawn Position", fountainPos, 0.0f, 1.0f);
				ImGui::SliderFloat3("Random Angle Range", fountainVelRange,0, 10);
				ImGui::SliderFloat3("Speed Intensity", fountainVelIntensity, -10, 10);
			}
			if (isCascadeMode){
				ImGui::SliderFloat3("Cascade Position", cascadePos, 0, 1.0f);
				ImGui::SliderFloat("Z Angle Range", &cascadeVelZRange, 0.0f, 10.0f);
				ImGui::SliderFloat("Z Force", &cascadeVelZ, -10.0f, 10.0f);
			}
			ImGui::TreePop();
		}
		
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if (show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);

	}
}	 
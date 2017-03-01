#pragma once
#include <iostream>


struct Particles{
	float pos[3];
	float speed;
	float lifeTime;
};


class ParticlesSetup {
public:
	
	ParticlesSetup() {};
	
	void newParticles(float posX, float posY, float posZ, int maxPart) {
		Particles ToIns;
		ToIns.pos[0] = posX;
		ToIns.pos[1] = posY;
		ToIns.pos[2] = posZ;

		for (int i = 0; i < maxPart; i++) {
			arrPart[i] = ToIns;
		}
	}

	~ParticlesSetup() {};

private:
	Particles arrPart[10000];
};
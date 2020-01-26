#ifndef PARTICLE_H
#define PARTICLE_H

#include "model.h"

#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>

class Particle
{
public:
	Particle(Model& particleMesh, Shader& particleShader, float mass, glm::vec3 position, glm::vec3 velocity, float totalTime);
	void update(float dt);

private:
	Model particleMesh;
	Shader particleShader;
	float mass;
	glm::vec3 position, velocity;

	float totalTime, currentTime;
	glm::vec3 computeGravity();
};

#endif

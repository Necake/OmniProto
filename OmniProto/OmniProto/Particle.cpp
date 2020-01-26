#include "Particle.h"

Particle::Particle(Model& particleMesh, Shader& particleShader, float mass, glm::vec3 position, glm::vec3 velocity, float totalTime):
	particleMesh(particleMesh), particleShader(particleShader), mass(mass), position(position), velocity(velocity), totalTime(totalTime)
{
	currentTime = 0;
}

void Particle::update(float dt)
{
	currentTime += dt;

	glm::vec3 gravityForce = computeGravity();
	velocity += (gravityForce * dt / mass);
	position += (velocity * dt);

	particleShader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(.01f, .01f, .01f));
	particleShader.setMat4("model", model);
	particleMesh.draw(particleShader);
}

glm::vec3 Particle::computeGravity()
{
	return glm::vec3(0, -9.81f * mass, 0);
}
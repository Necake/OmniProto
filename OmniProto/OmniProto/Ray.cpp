#include "Ray.h"

Ray::Ray(): 
	origin(glm::vec3(0,0,0)), direction(glm::vec3(1,0,0)), color(glm::vec3(0,0,0))
{
}

Ray::Ray(glm::vec3 origin, glm::vec3 direction, glm::vec3 color): 
	origin(origin), direction(direction), color(color)
{ //Set class variables and generate ray
	generate(); 
}

void Ray::draw(Shader& shader)
{
	shader.use();
	shader.setVec3("color", color);
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 2); //Draw ray as a line
}

void Ray::generate()
{
	glm::vec3 vertices[] = { origin, origin + direction }; //Declare 2 vertices as a start and endpoint for the line
	glGenVertexArrays(1, &this->VAO); //Generate VAO/VBO, trivial stuff
	glGenBuffers(1, &this->VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //Position vertex attribute
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind everything
}
#ifndef RAY_H
#define RAY_H

#include<GLAD\glad.h>
#include<GLFW\glfw3.h>
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>
#include<glm\gtc\constants.hpp>

#include "shader.h"
class Ray
{
public:
	Ray();
	Ray(glm::vec3 origin, glm::vec3 direction, glm::vec3 color);
	void draw(Shader& shader);
	void generate();
private:
	unsigned int VAO, VBO;
	glm::vec3 origin, direction, color;
};

#endif
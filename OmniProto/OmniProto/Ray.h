//========================================================================================
//Ray class - used for drawing rays
//
// Nemanja Milanovic, 2020 / neca1mesto@gmail.com / github.com/Necake
//========================================================================================

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
	Ray(); //Blank constructor
	Ray(glm::vec3 origin, glm::vec3 direction, glm::vec3 color); //Constructor based on origin, direction (and intensity), and displayed color
	void draw(Shader& shader); //Draw ray with the appropriate shader
	void generate(); //Generate ray geometry with given VAO/VBO
private:
	unsigned int VAO, VBO;
	glm::vec3 origin, direction, color;
};

#endif
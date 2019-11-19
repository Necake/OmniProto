#ifndef RAYUTIL_H
#define RAYUTIL_H

#define __EPSILON 0.00001f

#include<GLAD\glad.h>
#include<GLFW\glfw3.h>
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>
#include<glm\gtc\constants.hpp>

#include<iostream>
#include<string>
#include "shader.h"

namespace RayUtil
{
	const glm::vec3 xOrigin(-1000, 0, 0);
	const glm::vec3 xDirection(2000, 0, 0);
	const glm::vec3 yOrigin(0, -1000, 0);
	const glm::vec3 yDirection(0, 2000, 0);
	const glm::vec3 zOrigin(0, 0, -1000);
	const glm::vec3 zDirection(0, 0, 2000);

	void renderRay(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::mat4 view, glm::mat4 model, glm::mat4 projection, Shader& shader, glm::vec3 color)
	{
		//Render the ray (debug purposes)
		glm::vec3 vertices[] = { rayOrigin, rayOrigin + rayDir };
		unsigned int VAO, VBO;
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //position vertex attribute
		glEnableVertexAttribArray(0);

		shader.use();
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("color", color);
		glDrawArrays(GL_LINES, 0, 2);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}

	void renderAxes(glm::mat4 view, glm::mat4 model, glm::mat4 projection, Shader& shader)
	{
		renderRay(xOrigin, xDirection, view, model, projection, shader, glm::vec3(1, 0, 0));
		renderRay(yOrigin, yDirection, view, model, projection, shader, glm::vec3(0, 1, 0));
		renderRay(zOrigin, zDirection, view, model, projection, shader, glm::vec3(0, 0, 1));
	}

}

#endif
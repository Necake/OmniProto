//========================================================================================
//RayUtil namespace - Contains useful functions for manipulating rays
//
// Nemanja Milanovic, 2020 / neca1mesto@gmail.com / github.com/Necake
//========================================================================================
#ifndef RAYUTIL_H
#define RAYUTIL_H

#define __EPSILON 0.00001f
#include "Ray.h"
namespace RayUtil
{
	Ray xAxis, yAxis, zAxis;
	void initAxes() //Construct rays for the x, y, and z axis (see Ray class)
	{
		xAxis = Ray(glm::vec3(-1000, 0, 0), glm::vec3(2000, 0, 0), glm::vec3(1, 0, 0));
		yAxis = Ray(glm::vec3(0, -1000, 0), glm::vec3(0, 2000, 0), glm::vec3(0, 1, 0));
		zAxis = Ray(glm::vec3(0, 0, -1000), glm::vec3(0, 0, 2000), glm::vec3(0, 0, 1));
	}
	void renderAxes(glm::mat4 model, Shader& shader) //Render XYZ axes
	{
		shader.use();
		shader.setMat4("model", model);
		shader.setVec3("color", glm::vec3(1, 0, 0));
		xAxis.draw(shader);
		yAxis.draw(shader);
		zAxis.draw(shader);
	}

}

#endif
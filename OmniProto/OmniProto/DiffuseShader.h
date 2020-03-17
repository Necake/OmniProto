//========================================================================================
//Diffuse shader class - Experimental, for managing a common type of shader in order 
// to spare the constant searching for variable locations in the gpu
//
// Nemanja Milanovic, 2020 / neca1mesto@gmail.com / github.com/Necake
//========================================================================================

#ifndef DIFFUSE_H
#define DIFFUSE_H

#include "Shader.h"

//========================================================================================
// The diffuse shader has a structure as follows:
// >a material with the following params:
//    >3 material vec3 colors (ambient/diffuse/specular) (these can also be textures!)
//    >specular shininess, float value (2^n)
// >a single point light with the following params:
//    >3 vec3 colors (ambient/diffuse/specular)
//    >attenuation float values (constant/linear/quadratic)
// >a single directional light with the following params:
//    >3 vec3 colors (ambient/diffuse/specular)
//    >vec3 direction
// >a single flashlight with the following params:
//    >3 vec3 colors (ambient/diffuse/specular)
//    >attenuation float values (constant/linear/quadratic)
//    >vec3 position, direction
//    >float cutoff and outer cutoff angles
// >some general params:
//========================================================================================


class DiffuseShader : public Shader
{
public:
	bool isTextured;
	unsigned int mat
	DiffuseShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr) : 
		Shader(vertexPath, fragmentPath, geometryPath)
	{

	}

};

#endif
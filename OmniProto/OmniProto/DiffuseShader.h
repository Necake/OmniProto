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
//    >mat4 model, view, projection (view and projection shared through uniform block binding)
//========================================================================================


class DiffuseShader : public Shader
{
public:
	bool isTextured;
	unsigned int matAmbient, matDiffuse, matSpecular, matShine;
	unsigned int plAmbient, plDiffuse, plSpecular, plConst, plLinear, plQuadratic, plPosition;
	unsigned int dlAmbient, dlDiffuse, dlSpecular, dlDirection;
	unsigned int flAmbient, flDiffuse, flSpecular, flConst, flLinear, flQuadratic, flPosition, flDirection, flCutoff, flOuterCutoff;
	unsigned int model, viewPos, skybox;
	DiffuseShader(bool isTextured) : 
		Shader("../OmniProto/diffuse.vert", "../OmniProto/diffuse_tex.frag", "../OmniProto/diffuse.geom"), isTextured(isTextured)
	{
		matAmbient = glGetUniformLocation(this->ID, "material.ambient");
		if (isTextured)
		{
			matDiffuse = glGetUniformLocation(this->ID, "material.texture_diffuse1");
			matSpecular = glGetUniformLocation(this->ID, "material.texture_specular1");
		}
		else
		{
			matDiffuse = glGetUniformLocation(this->ID, "material.diffuse");
			matSpecular = glGetUniformLocation(this->ID, "material.specular");
		}
		matShine = glGetUniformLocation(this->ID, "material.shininess");

		plAmbient = glGetUniformLocation(this->ID, "pointLight.ambient");
		plDiffuse = glGetUniformLocation(this->ID, "pointLight.diffuse");
		plSpecular = glGetUniformLocation(this->ID, "pointLight.specular");
		plConst = glGetUniformLocation(this->ID, "pointLight.constant");
		plLinear = glGetUniformLocation(this->ID, "pointLight.linear");
		plQuadratic = glGetUniformLocation(this->ID, "pointLight.quadratic");
		plPosition = glGetUniformLocation(this->ID, "pointLight.position");

		dlDirection = glGetUniformLocation(this->ID, "dirLight.direction");
		dlAmbient = glGetUniformLocation(this->ID, "dirLight.ambient");
		dlDiffuse = glGetUniformLocation(this->ID, "dirLight.diffuse");
		dlSpecular = glGetUniformLocation(this->ID, "dirLight.specular");

		flPosition = glGetUniformLocation(this->ID, "flashLight.position");
		flDirection = glGetUniformLocation(this->ID, "flashLight.direction");
		flCutoff = glGetUniformLocation(this->ID, "flashLight.cutoff");
		flOuterCutoff = glGetUniformLocation(this->ID, "flashLight.outerCutoff");
		flAmbient = glGetUniformLocation(this->ID, "flashLight.ambient");
		flDiffuse = glGetUniformLocation(this->ID, "flashLight.diffuse");
		flSpecular = glGetUniformLocation(this->ID, "flashLight.specular");
		flConst = glGetUniformLocation(this->ID, "flashLight.constant");
		flLinear = glGetUniformLocation(this->ID, "flashLight.linear");
		flQuadratic = glGetUniformLocation(this->ID, "flashLight.quadratic");

		viewPos = glGetUniformLocation(this->ID, "viewPos");
		model = glGetUniformLocation(this->ID, "model");
		skybox = glGetUniformLocation(this->ID, "skybox");

	}

	void setupGeneral(glm::vec3& viewPos, glm::mat4& model, int skybox)
	{
		glUniform3fv(this->viewPos, 1, &viewPos[0]);
		glUniformMatrix4fv(this->model, 1, GL_FALSE, &model[0][0]);
		glUniform1i(this->skybox, skybox);
	}

	void setupMaterial(glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular, float shine)
	{
		if (!isTextured)
		{
			glUniform3fv(matAmbient, 1, &ambient[0]);
			glUniform3fv(matDiffuse, 1, &diffuse[0]);
		}
		glUniform3fv(matSpecular, 1, &specular[0]);
		glUniform1f(matShine, shine);
	}

	void setupPointLight(glm::vec3& pos, glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular, float constant, float linear, float quadratic)
	{
		glUniform3fv(plPosition, 1, &pos[0]);
		glUniform3fv(plAmbient, 1, &ambient[0]);
		glUniform3fv(plDiffuse, 1, &diffuse[0]);
		glUniform3fv(plSpecular, 1, &specular[0]);
		glUniform1f(plConst, constant);
		glUniform1f(plLinear, linear);
		glUniform1f(plQuadratic, quadratic);
	}
	
	void setupDirecitonalLight(glm::vec3& dir, glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular)
	{
		glUniform3fv(dlDirection, 1, &dir[0]);
		glUniform3fv(dlAmbient, 1, &ambient[0]);
		glUniform3fv(dlDiffuse, 1, &diffuse[0]);
		glUniform3fv(dlSpecular, 1, &specular[0]);
	}

	void setupFlashLight(glm::vec3& pos, glm::vec3& dir, glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular, float constant, float linear, float quadratic, float cutoff, float outerCutoff)
	{
		glUniform3fv(flPosition, 1, &pos[0]);
		glUniform3fv(flDirection, 1, &dir[0]);
		glUniform3fv(flAmbient, 1, &ambient[0]);
		glUniform3fv(flDiffuse, 1, &diffuse[0]);
		glUniform3fv(flSpecular, 1, &specular[0]);
		glUniform1f(flConst, constant);
		glUniform1f(flLinear, linear);
		glUniform1f(flQuadratic, quadratic);
		glUniform1f(flCutoff, cutoff);
		glUniform1f(flOuterCutoff, outerCutoff);
	}

};

#endif
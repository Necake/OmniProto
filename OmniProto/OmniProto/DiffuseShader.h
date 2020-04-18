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

struct DiffuseShaderUniforms
{
	unsigned int matAmbient, matDiffuse, matSpecular, matShine;
	unsigned int plAmbient, plDiffuse, plSpecular, plConst, plLinear, plQuadratic, plPosition;
	unsigned int dlAmbient, dlDiffuse, dlSpecular, dlDirection;
	unsigned int flAmbient, flDiffuse, flSpecular, flConst, flLinear, flQuadratic, flPosition, flDirection, flCutoff, flOuterCutoff;
	unsigned int model, viewPos, skybox;
};


class DiffuseShader : public Shader
{
public:
	bool isTextured;

	glm::vec3 matAmbient, matDiffuse, matSpecular;
	float matShine;
	
	glm::vec3 plAmbient, plDiffuse, plSpecular, plPosition;
	float plConst, plLinear, plQuadratic;

	glm::vec3 dlAmbient, dlDiffuse, dlSpecular, dlDirection;

	glm::vec3 flAmbient, flDiffuse, flSpecular, flPosition, flDirection;
	float flConst, flLinear, flQuadratic, flCutoff, flOuterCutoff;
	//====================================================================================================
	DiffuseShader(bool isTextured) : 
		Shader("../OmniProto/diffuseShadows.vert", "../OmniProto/diffuseShadows.frag", "../OmniProto/diffuseShadows.geom"), isTextured(isTextured)
	{
		uniforms.matAmbient = glGetUniformLocation(this->ID, "material.ambient");
		if (isTextured)
		{
			uniforms.matDiffuse = glGetUniformLocation(this->ID, "material.texture_diffuse1");
			uniforms.matSpecular = glGetUniformLocation(this->ID, "material.texture_specular1");
		}
		else
		{
			uniforms.matDiffuse = glGetUniformLocation(this->ID, "material.diffuse");
			uniforms.matSpecular = glGetUniformLocation(this->ID, "material.specular");
		}
		uniforms.matShine = glGetUniformLocation(this->ID, "material.shininess");

		uniforms.plAmbient = glGetUniformLocation(this->ID, "pointLight.ambient");
		uniforms.plDiffuse = glGetUniformLocation(this->ID, "pointLight.diffuse");
		uniforms.plSpecular = glGetUniformLocation(this->ID, "pointLight.specular");
		uniforms.plConst = glGetUniformLocation(this->ID, "pointLight.constant");
		uniforms.plLinear = glGetUniformLocation(this->ID, "pointLight.linear");
		uniforms.plQuadratic = glGetUniformLocation(this->ID, "pointLight.quadratic");
		uniforms.plPosition = glGetUniformLocation(this->ID, "pointLight.position");

		uniforms.dlDirection = glGetUniformLocation(this->ID, "dirLight.direction");
		uniforms.dlAmbient = glGetUniformLocation(this->ID, "dirLight.ambient");
		uniforms.dlDiffuse = glGetUniformLocation(this->ID, "dirLight.diffuse");
		uniforms.dlSpecular = glGetUniformLocation(this->ID, "dirLight.specular");

		uniforms.flPosition = glGetUniformLocation(this->ID, "flashLight.position");
		uniforms.flDirection = glGetUniformLocation(this->ID, "flashLight.direction");
		uniforms.flCutoff = glGetUniformLocation(this->ID, "flashLight.cutoff");
		uniforms.flOuterCutoff = glGetUniformLocation(this->ID, "flashLight.outerCutoff");
		uniforms.flAmbient = glGetUniformLocation(this->ID, "flashLight.ambient");
		uniforms.flDiffuse = glGetUniformLocation(this->ID, "flashLight.diffuse");
		uniforms.flSpecular = glGetUniformLocation(this->ID, "flashLight.specular");
		uniforms.flConst = glGetUniformLocation(this->ID, "flashLight.constant");
		uniforms.flLinear = glGetUniformLocation(this->ID, "flashLight.linear");
		uniforms.flQuadratic = glGetUniformLocation(this->ID, "flashLight.quadratic");

		uniforms.viewPos = glGetUniformLocation(this->ID, "viewPos");
		uniforms.model = glGetUniformLocation(this->ID, "model");
		uniforms.skybox = glGetUniformLocation(this->ID, "skybox");

	}

	void setupGeneral(glm::vec3& viewPos, glm::mat4& model, int skybox)
	{
		glUniform3fv(this->uniforms.viewPos, 1, &viewPos[0]);
		glUniformMatrix4fv(this->uniforms.model, 1, GL_FALSE, &model[0][0]);
		glUniform1i(this->uniforms.skybox, skybox);
	}

	void updateMaterial()
	{
		if (!isTextured)
		{
			glUniform3fv(uniforms.matAmbient, 1, &matAmbient[0]);
			glUniform3fv(uniforms.matDiffuse, 1, &matDiffuse[0]);
		}
		glUniform3fv(uniforms.matSpecular, 1, &matSpecular[0]);
		glUniform1f(uniforms.matShine, matShine);
	}

	void updatePointLight()
	{
		glUniform3fv(uniforms.plPosition, 1, &plPosition[0]);
		glUniform3fv(uniforms.plAmbient, 1, &plAmbient[0]);
		glUniform3fv(uniforms.plDiffuse, 1, &plDiffuse[0]);
		glUniform3fv(uniforms.plSpecular, 1, &plSpecular[0]);
		glUniform1f(uniforms.plConst, plConst);
		glUniform1f(uniforms.plLinear, plLinear);
		glUniform1f(uniforms.plQuadratic, plQuadratic);
	}
	
	void updateDirecitonalLight()
	{
		glUniform3fv(uniforms.dlDirection, 1, &dlDirection[0]);
		glUniform3fv(uniforms.dlAmbient, 1, &dlAmbient[0]);
		glUniform3fv(uniforms.dlDiffuse, 1, &dlDiffuse[0]);
		glUniform3fv(uniforms.dlSpecular, 1, &dlSpecular[0]);
	}

	void setupFlashLight(glm::vec3 position, glm::vec3 direction)
	{
		glUniform3fv(uniforms.flPosition, 1, &position[0]);
		glUniform3fv(uniforms.flDirection, 1, &direction[0]);
		glUniform3fv(uniforms.flAmbient, 1, &flAmbient[0]);
		glUniform3fv(uniforms.flDiffuse, 1, &flDiffuse[0]);
		glUniform3fv(uniforms.flSpecular, 1, &flSpecular[0]);
		glUniform1f(uniforms.flConst, flConst);
		glUniform1f(uniforms.flLinear, flLinear);
		glUniform1f(uniforms.flQuadratic, flQuadratic);
		glUniform1f(uniforms.flCutoff, flCutoff);
		glUniform1f(uniforms.flOuterCutoff, flOuterCutoff);
	}

private:
	DiffuseShaderUniforms uniforms;
};

#endif
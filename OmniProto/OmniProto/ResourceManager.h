#ifndef RESOURCE_H
#define RESOURCE_H

#include <map>
#include <string>
#include "Texture.h"
#include "shader.h"
#include "model.h"

using namespace std;

class ResourceManager
{
public:
	static map<string, Texture> Textures;
	static map<string, Shader> Shaders;
	static map<string, Model> Models;

	static Texture& loadTexture(const char* texturePath, std::string name);
	static Texture& getTexture(std::string name);
	static Model& loadModel(const char* modelPath, std::string name);
	static Model& getModel(std::string name);

	static void Clear();
private:
	ResourceManager();
};

#endif
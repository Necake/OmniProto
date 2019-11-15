#ifndef RESOURCE_H
#define RESOURCE_H

#include <map>
#include <string>
#include "Texture.h"
#include "shader.h"

using namespace std;

class ResourceManager
{
public:
	static map<string, Texture> Textures;
	static map<string, Shader> Shaders;

	static Texture& loadTexture(const char* texturePath, std::string name);
	static Texture& getTexture(std::string name);

	static void Clear();
private:
	ResourceManager();
};

#endif
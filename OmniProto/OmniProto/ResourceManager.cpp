#include "ResourceManager.h"

map<string, Texture> ResourceManager::Textures;
map<string, Shader> ResourceManager::Shaders;

Texture& ResourceManager::loadTexture(const char* texturePath, std::string name)
{
	Texture texture(texturePath);
	Textures[name] = texture;
	return Textures[name];
}

Texture& ResourceManager::getTexture(std::string name)
{
	return Textures[name];
}

void ResourceManager::Clear()
{
	// (Properly) delete all shaders	
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	// (Properly) delete all textures
	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);
}
#include "ResourceManager.h"

map<string, Texture> ResourceManager::Textures;
map<string, Shader> ResourceManager::Shaders;
map<string, Model> ResourceManager::Models;

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

Model& ResourceManager::loadModel(const char* modelPath, std::string name)
{
	Model model(modelPath);
	Models[name] = model;
	return Models[name];
}

Model& ResourceManager::getModel(std::string name)
{
	return Models[name];
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
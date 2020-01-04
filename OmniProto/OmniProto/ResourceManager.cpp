#include "ResourceManager.h"

map<string, Texture> ResourceManager::Textures; //Instantiate static maps
map<string, Shader> ResourceManager::Shaders;
map<string, Model> ResourceManager::Models;

Texture& ResourceManager::loadTexture(const char* texturePath, std::string name)
{
	Texture texture(texturePath); //Construct texture from path (see Texture class)
	Textures[name] = texture;
	return Textures[name]; //Store the texture in the map and return
}

Texture& ResourceManager::getTexture(std::string name)
{
	return Textures[name]; //Return texture from map
}

Model& ResourceManager::loadModel(const char* modelPath, std::string name)
{
	Model model(modelPath); //Construct model, store in map, return reference (see Model class for construction)
	Models[name] = model;
	return Models[name];
}

Model& ResourceManager::getModel(std::string name)
{
	return Models[name]; //Return ref from map
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
//========================================================================================
//ResourceManager class - static class used for handling loading textures, models
// and shaders(TODO). Assets are stored in maps and queued by given name
//
// Nemanja Milanovic, 2020 / neca1mesto@gmail.com / github.com/Necake
//========================================================================================

#ifndef RESOURCE_H
#define RESOURCE_H

#include <map>
#include <string>
#include "Texture.h"
#include "Shader.h"
#include "model.h"

using namespace std;

class ResourceManager
{
public:
	static map<string, Texture> Textures; //Maps for storing assets
	static map<string, Shader> Shaders;
	static map<string, Model> Models;

	static Texture& loadTexture(const char* texturePath, std::string name); //Loading textures and models, returns reference to said asset
	static Texture& getTexture(std::string name);
	static Model& loadModel(const char* modelPath, std::string name);
	static Model& getModel(std::string name);

	static void Clear(); //Clear out all data
private:
	ResourceManager(); //Static class, disable instancing
};

#endif
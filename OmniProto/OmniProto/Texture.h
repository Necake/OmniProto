//========================================================================================
//Texture class - Used for generating OpenGL Texture2D objects from raw file path, 
// texture loading is done with STB_IMAGE through the ImageLoader class
//
// Nemanja Milanovic, 2020 / neca1mesto@gmail.com / github.com/Necake
//========================================================================================

#ifndef TEXTURE_H
#define TEXTURE_H

#include<GLAD/glad.h>
#include<iostream>


class Texture
{
public:
	unsigned int ID; //OpenGL object ID
	int width, height; //Width and height of thexture
	unsigned int internalFormat, imageFormat; //Internal and loaded image format
	unsigned int wrapS, wrapT, filterMin, filterMax; //Wrapping and scaling filter params
	Texture(const char* path); //Construct from path
	Texture(); //Blank constructor
	void bind() const; //Bind opengl texture
	unsigned int generateModelTexture(const char* path, const std::string& directory, bool gamma); //Generate textures for a model, should prob move this to the model class
private:
	void generate(const char* path); //Generate OpenGL object from path, called from constructor
};

#endif
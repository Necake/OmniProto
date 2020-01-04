//========================================================================================
//Cubemap class - used for drawing OpenGL cubemaps using 6 provided textures
// textures are loaded using stb image through the ImageLoader class
//
// Nemanja Milanovic, 2020 / neca1mesto@gmail.com / github.com/Necake
//========================================================================================

#ifndef CUBEMAP_H
#define CUBEMAP_H

#include<GLAD/glad.h>
#include<iostream>

class Cubemap
{
public:
	unsigned int ID; //OpenGL object ID
	int width, height; //Width and height of textures (assumed to be the same for all 6)
	unsigned int internalFormat, imageFormat; //Internal and loaded image formats
	unsigned int wrapS, wrapT, wrapR, filterMin, filterMax; //Wrapping and filters
	Cubemap(const char* paths[6]); //Creates a cubemap from provided textures
	Cubemap(); //Blank cubemap
	void bind() const; //Bind the OpenGL cubemap to this cubemap
private:
	void generate(const char* paths[6]); //Called from the constructor
};

#endif
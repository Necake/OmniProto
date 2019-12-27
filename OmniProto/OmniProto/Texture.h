#ifndef TEXTURE_H
#define TEXTURE_H

#include<GLAD/glad.h>
#include<iostream>

class Texture
{
public:
	unsigned int ID;
	int width, height;
	unsigned int internalFormat, imageFormat;
	unsigned int wrapS, wrapT, filterMin, filterMax;
	Texture(const char* path);
	Texture();
	static int createSkybox(const char* [6]);
	void bind() const;
	unsigned int generateModelTexture(const char* path, const std::string& directory, bool gamma);
private:
	void generate(const char* path);
};

#endif
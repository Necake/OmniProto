#ifndef CUBEMAP_H
#define CUBEMAP_H

#include<GLAD/glad.h>
#include<iostream>

class Cubemap
{
public:
	unsigned int ID;
	int width, height;
	unsigned int internalFormat, imageFormat;
	unsigned int wrapS, wrapT, wrapR, filterMin, filterMax;
	Cubemap(const char* paths[6]);
	Cubemap();
	void bind() const;
private:
	void generate(const char* paths[6]);
};

#endif
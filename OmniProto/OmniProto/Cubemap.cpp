#include "Cubemap.h"
#include "ImageLoader.h"

Cubemap::Cubemap(const char* paths[6]) : wrapS(GL_CLAMP_TO_EDGE), wrapR(GL_CLAMP_TO_EDGE), wrapT(GL_CLAMP_TO_EDGE), filterMax(GL_LINEAR), filterMin(GL_LINEAR)
{
	//Constructor uses assumed wrapping and filtering methods
	generate(paths); //Generate textures
}

Cubemap::Cubemap() : wrapS(GL_CLAMP_TO_EDGE), wrapR(GL_CLAMP_TO_EDGE), wrapT(GL_CLAMP_TO_EDGE), filterMax(GL_LINEAR), filterMin(GL_LINEAR), width(0), height(0)
{
}

void Cubemap::bind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

void Cubemap::generate(const char* paths[6])
{
	ImageLoader::flipVertical = false; 
	glGenTextures(1, &ID); //Generate OpenGL cubemap
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

	int nrChans;
	for (int i = 0; i < 6; i++)
	{
		unsigned char* data = ImageLoader::loadImage(paths[i], &width, &height, &nrChans); //Load the 6 images for the cubemap
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); //Bind the texture into the cubemap accoringly
			internalFormat = GL_RGB;
			imageFormat = GL_RGB;
			ImageLoader::freeImage(data); //Free the raw data
		}
		else
		{
			std::cout << "Failed to load cubemap!\n";
			ImageLoader::freeImage(data);
		}
	}
	//Set filtering and wrapping parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filterMin);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filterMax);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapT);
}

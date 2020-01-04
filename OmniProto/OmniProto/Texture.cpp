#include "Texture.h"

#include "ImageLoader.h"

Texture::Texture(const char* path) : width(0), height(0), internalFormat(GL_RGBA), imageFormat(GL_RGBA),
wrapS(GL_CLAMP_TO_EDGE), wrapT(GL_CLAMP_TO_EDGE), filterMin(GL_LINEAR_MIPMAP_LINEAR), filterMax(GL_LINEAR)
{
	glGenTextures(1, &this->ID); //Generate with the class ID, call generate function, wrapping and scaling params assumed
	generate(path);
}

Texture::Texture() : width(0), height(0), internalFormat(GL_RGBA), imageFormat(GL_RGBA),
wrapS(GL_CLAMP_TO_EDGE), wrapT(GL_CLAMP_TO_EDGE), filterMin(GL_LINEAR_MIPMAP_LINEAR), filterMax(GL_LINEAR)
{
}

void Texture::generate(const char* path)
{
	glBindTexture(GL_TEXTURE_2D, ID); //Bind texture, set wrapping/scaling params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMax);
	int width, height, nrChans;
	ImageLoader::flipVertical = true;
	unsigned char* texData = ImageLoader::loadImage(path, &width, &height, &nrChans); //Load image from file
	if (texData)
	{
		if (nrChans == 3) //Check wether the texture has an alpha channel
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
			internalFormat = GL_RGB; imageFormat = GL_RGB;
		}
		else if (nrChans == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
			internalFormat = GL_RGBA; imageFormat = GL_RGBA;
		}
		glGenerateMipmap(GL_TEXTURE_2D); //Generate texture mipmaps
		this->width = width; this->height = height; //Set width/height class values
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	ImageLoader::freeImage(texData); //Free raw image data
}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, ID);
}

unsigned int Texture::generateModelTexture(const char* path, const std::string& directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = ImageLoader::loadImage(filename.c_str(), &width, &height, &nrComponents);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		ImageLoader::freeImage(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		ImageLoader::freeImage(data);
	}

	return textureID;
}
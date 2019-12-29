#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool ImageLoader::flipVertical = false;

unsigned char* ImageLoader::loadImage(const char* path, int* width, int* height, int* nrChans)
{
	stbi_set_flip_vertically_on_load(flipVertical);
	return stbi_load(path, width, height, nrChans, 0);
}

void ImageLoader::freeImage(unsigned char* texData)
{
	stbi_image_free(texData);
}

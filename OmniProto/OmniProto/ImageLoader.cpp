#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool ImageLoader::flipVertical = false; //Define static bool variable

unsigned char* ImageLoader::loadImage(const char* path, int* width, int* height, int* nrChans)
{
	stbi_set_flip_vertically_on_load(flipVertical);
	return stbi_load(path, width, height, nrChans, 0); //Call stbi function for image loading
}

void ImageLoader::freeImage(unsigned char* texData)
{
	stbi_image_free(texData); //Call stbi function for releasing raw image data
}

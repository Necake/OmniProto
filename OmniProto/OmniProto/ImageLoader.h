//========================================================================================
//ImageLoader class - static class used for interacting with the STB_IMAGE library 
// (pretty much a wrapper), since the lib can only be included in one cpp file
//
// Nemanja Milanovic, 2020 / neca1mesto@gmail.com / github.com/Necake
//========================================================================================

#ifndef IMGLOADER_H
#define IMGLOADER_H

class ImageLoader
{
public:
	static unsigned char* loadImage(const char* path, int* width, int* height, int* nrChans); //Load image from path and store width, height, and channel data
	static void freeImage(unsigned char* texData); //Free raw image data
	static bool flipVertical; //Flag for flipping textures vertically, used pretty often
private:
	ImageLoader(); //Static class, disable instancing
};


#endif

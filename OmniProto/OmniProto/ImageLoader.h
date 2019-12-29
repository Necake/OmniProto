#ifndef IMGLOADER_H
#define IMGLOADER_H
class ImageLoader
{
public:
	static unsigned char* loadImage(const char* path, int* width, int* height, int* nrChans);
	static void freeImage(unsigned char* texData);
	static bool flipVertical;
private:
	ImageLoader();
};


#endif

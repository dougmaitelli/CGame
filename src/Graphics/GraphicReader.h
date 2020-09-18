#ifndef GAME_PTM_READER_H
#define GAME_PTM_READER_H

#include <fstream>

#include "../../include/IL/il.h"

#include "Image.h"

using namespace std;

class GraphicReader {
public:
	static Image* readImage(char* filename) {
		ILuint imageName;

		ilGenImages(1, &imageName);
		ilBindImage(imageName);

		ilLoadImage(filename);

		ILuint width, height;
		width = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);

		Image* image = new Image(width, height);

		unsigned char* pixmap = new BYTE[width * height * 4];
		ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, pixmap);

		int a, r, g, b;
			
		for (int y = 0; y < (*image).getHeight(); y++)
			for (int x = 0; x < (*image).getWidth(); x++) {
				r = pixmap[(x + y * width) * 4 + 0];
				g = pixmap[(x + y * width) * 4 + 1];
				b = pixmap[(x + y * width) * 4 + 2];
				a = pixmap[(x + y * width) * 4 + 3];

				(*image).setPixel(a, r, g, b, x, (*image).getHeight() - y - 1);
			}

		ilBindImage(0);
		ilDeleteImages(1, &imageName);

		delete pixmap;

		return image;
	}
}; 

#endif

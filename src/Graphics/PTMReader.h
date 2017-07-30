#ifndef GAME_PTM_READER_H
#define GAME_PTM_READER_H

#include <fstream>
#include <string>

#include "Image.h"

using namespace std;

class PTMReader {
public:
	static Image * read(char *filename) {
		ifstream arq(filename);

		char buffer[256];
		int w, h, mv, a, r, g, b;
		string tipo;

		arq >> buffer;
		tipo = buffer;

		arq >> buffer;
		if (buffer == "#")
			arq >> w;
		else
			w = atoi(buffer);
		arq >> h;
		arq >> mv;

		Image *imagem = new Image(w, h);

		for (int y = (*imagem).getHeight() - 1; y >= 0; y--)
			for (int x = 0; x < (*imagem).getWidth(); x++) {
				arq >> a;
				arq >> r;
				arq >> g;
				arq >> b;
				(*imagem).setPixel(a, r, g, b, x, y);
			}

		return imagem;
	}
}; 

#endif
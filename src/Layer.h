#ifndef GAME_LAYER_H
#define GAME_LAYER_H

#include "Graphics/Image.h"

class Layer {
public:
	Layer() {}

	Layer(Image* image, unsigned int zIndex) {
		this->image = image;
		this->zIndex = zIndex;
	}

	void setImagem(Image* image) {
		this->image = image;
	}

	Image* getImage() {
		return image;
	}

	float getTaxaX() {
		return 1.0 - (float) zIndex * 0.05;
	}

	float getPosX() {
		return posX;
	}

	void setPosX(float pos) {
		posX = pos;
	}
	
	void scroll(bool plus, int valor) {
		if (plus) 
			posX -= valor * getTaxaX();
		else 
			posX += valor * getTaxaX();
		if (posX < 0)
			posX = 0;
	}

private:
	Image* image;
	unsigned int zIndex = 0;

	float posX = 0;
};

#endif
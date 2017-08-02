#ifndef GAME_LAYER_H
#define GAME_LAYER_H

#include "Graphics/Image.h"

class Layer {
public:
	void setImagem(Image* image) {
		this->image = image;
	}

	Image* getImage() {
		return image;
	}

	float getTaxaX() {
		return taxaX;
	}

	void setTaxaX(float tx) {
		taxaX = tx;
	}

	float getPosX() {
		return posX;
	}

	void setPosX(float pos) {
		posX = pos;
	}
	
	void scroll(bool plus, int valor) {
		if (plus) 
			posX -= valor * taxaX;
		else 
			posX += valor * taxaX;
		if (posX < 0)
			posX = 0;
	}

private:
	Image* image;

	float taxaX = 0;
	float posX = 0;
};

#endif
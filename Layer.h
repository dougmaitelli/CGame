class Layer {
public:
	void setImagem(Image *image) {
		imagem = image;
	}

	Image * getImagem(void) {
		return imagem;
	}

	float getTaxaX(void) {
		return taxaX;
	}

	void setTaxaX(float tx) {
		taxaX = tx;
	}

	float getPosX(void) {
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
	Image *imagem;
	float taxaX = 0;
	float posX = 0;
};
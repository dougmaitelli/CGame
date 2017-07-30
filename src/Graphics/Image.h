#ifndef GAME_IMAGE_H
#define GAME_IMAGE_H

class Image {
public:
	Image(int w, int h) {
		width = w; height = h;
		pixels = new int[w*h];
	}

	void setPixel(int argb, int x, int y) {
	pixels[x + y*width] = argb;
	}

	void setPixel(int r, int g, int b, int x, int y) {
		int rgb = (r << 16) | (g << 8) | b;
		pixels[x + y*width] = rgb;
	}

	void setPixel(int a, int r, int g, int b, int x, int y) {
		int argb = (a << 24) | (r << 16) | (g << 8) | b;
		pixels[x + y*width] = argb;
	}

	int getPixel(int x, int y) {
		return pixels[x + y*width];
	}

	int getPixelRepeat(int x, int y) {
		if (x >= 0 && x < width && y >= 0 && y < height)
			return pixels[x + y*width];
		else if (y < 0 || y >= height)
			return 0; //Não repetir imagens com menor altura
		else {
			int j, k;
			j = x / width;
			k = y / height;
			x -= width * j;
			y -= height * k;
			return pixels[x + y*width];
		}
	}

	void preenche(int argb, float prob) {
		int r = (argb >> 16) & 0xff;
		int g = (argb >> 8) & 0xff;
		int b = argb & 0xff;
		int a;
		for (int x = 0; x < (*this).getWidth(); x++) {
			for (int y = 0; y < (*this).getHeight(); y++) {
				a = (rand() % 2) > prob ? 1 : 0;
				if (a == 0)
					(*this).setPixel(0, r, g, b, x, y);
				else
					(*this).setPixel(255, r, g, b, x, y);
			}
		}
	}

	void plot(Image *foreground, int startx, int starty) {
		for (int y = 0; y < (*foreground).getHeight(); y++) {
			for (int x = 0; x < (*foreground).getWidth(); x++) {
				float a = (float) ((*foreground).getPixel(x, y) >> 24 & 0xff) / 255.0f;

				if (a == 1)
					(*this).setPixel((*foreground).getPixel(x, y), startx + x, starty + y);
				else if (a != 0) 
					(*this).interpolacaoCores(foreground, a, x, y, startx, starty, 0, 0);
			}
		}
	}

	void plotLayer(Image *foreground, int startxbg, int startybg, float startxfg, float startyfg) {
		for (int y = 0; y < (*foreground).getHeight(); y++) {
			for (int x = 0; x < (*this).getWidth(); x++) {
				float a = (float)((*foreground).getPixel(startxfg + x, startyfg + y) >> 24 & 0xff) / 255.0f;

				if (a == 1) 
					(*this).setPixel((*foreground).getPixel(startxfg + x, startyfg + y), startxbg + x, startybg + y);
				else if (a != 0) 
					(*this).interpolacaoCores(foreground, a, x, y, startxbg, startybg, startxfg, startyfg);
			}
		}
	}

	void plotLayerRepeat(Image *foreground, int startxbg, int startybg, float startxfg, float startyfg) {
		for (int y = 0; y < (*foreground).getHeight(); y++) {
			for (int x = 0; x < (*this).getWidth(); x++) {
				float a = (float)((*foreground).getPixelRepeat(startxfg + x, startyfg + y) >> 24 & 0xff) / 255.0f;

				if (a == 1)
					(*this).setPixel((*foreground).getPixelRepeat(startxfg + x, startyfg + y), startxbg + x, startybg + y);
				else if (a != 0)
					(*this).interpolacaoCores(foreground, a, x, y, startxbg, startybg, startxfg, startyfg);
			}
		}
	}

	void plotLayerZBuffer(Image *foreground, int startxbg, int startybg, float startxfg, float startyfg, int *zbuffer, int z) {
		for (int y = 0; y < (*foreground).getHeight(); y++) {
			for (int x = 0; x < (*this).getWidth(); x++) {
				float a = (float)((*foreground).getPixel(startxfg + x, startyfg + y) >> 24 & 0xff) / 255.0f;
				
				if (z >= zbuffer[(startxbg + x) + (startybg + y)*(*this).getWidth()]) {
					if (a == 1) {
						(*this).setPixel((*foreground).getPixel(startxfg + x, startyfg + y), startxbg + x, startybg + y);
						zbuffer[(startxbg + x) + (startybg + y)*(*this).getWidth()] = z;
					}
					else if (a != 0)
						(*this).interpolacaoCores(foreground, a, x, y, startxbg, startybg, startxfg, startyfg);
				}
			}
		}
	}

	void plotLayerZBufferRepeat(Image *foreground, int startxbg, int startybg, float startxfg, float startyfg, int *zbuffer, int z) {
		for (int y = 0; y < (*foreground).getHeight(); y++) {
			for (int x = 0; x < (*this).getWidth(); x++) {
				float a = (float)((*foreground).getPixelRepeat(startxfg + x, startyfg + y) >> 24 & 0xff) / 255.0f;

				if (z >= zbuffer[(startxbg + x) + (startybg + y)*(*this).getWidth()]) {
					if (a == 1) {
						(*this).setPixel((*foreground).getPixelRepeat(startxfg + x, startyfg + y), startxbg + x, startybg + y);
						zbuffer[(startxbg + x) + (startybg + y)*(*this).getWidth()] = z;
					}
					else if (a != 0)
						(*this).interpolacaoCores(foreground, a, x, y, startxbg, startybg, startxfg, startyfg);
				}
			}
		}
	}

	void plotAnim(Image *foreground, int startxbg, int startybg, float startxfg, float startyfg, int *zbuffer, int z) {
		for (int y = 0; y < (*foreground).getHeight(); y++) {
			for (int x = 0; x < (*foreground).getWidth(); x++) {
				float a = (float)((*foreground).getPixel(startxfg + x, startyfg + y) >> 24 & 0xff) / 255.0f;

				if (z >= zbuffer[(startxbg + x) + (startybg + y)*(*this).getWidth()]) {
					if (a == 1) 
						(*this).setPixel((*foreground).getPixel(startxfg + x, startyfg + y), startxbg + x, startybg + y);
					else if (a != 0)
						(*this).interpolacaoCores(foreground, a, x, y, startxbg, startybg, startxfg, startyfg);
				}
			}
		}
	}

	void interpolacaoCores(Image *foreground, float a, int x, int y, int startxbg, int startybg, int startxfg, int startyfg) {
		int argb1 = (*this).getPixel(x, y);
		int r1 = (argb1 >> 16) & 0xff;
		int g1 = (argb1 >> 8) & 0xff;
		int b1 = argb1 & 0xff;

		int argb2 = (*foreground).getPixel(startxfg + x, startyfg + y);
		int r2 = (argb2 >> 16) & 0xff;
		int g2 = (argb2 >> 8) & 0xff;
		int b2 = argb2 & 0xff;

		int r = r1*(1 - a) + r2*a;
		int g = g1*(1 - a) + g2*a;
		int b = b1*(1 - a) + b2*a;

		(*this).setPixel(255, r, g, b, startxbg + x, startybg + y);
	}

	void subImage(Image *dest, int startx, int starty) {
		for (int y = 0; y < (*dest).getHeight(); y++) {
			for (int x = 0; x < (*dest).getWidth(); x++) {
				int argbBG = (*this).getPixel(x + startx, y + starty);
				(*dest).setPixel(argbBG, x, y);
			}
		}
	}

	void copy(Image *imagem1) {
		for (int y = 0; y < (*this).getHeight(); y++) {
			for (int x = 0; x < (*this).getWidth(); x++) {
				int argb = (*this).getPixel(x, y);
				(*imagem1).setPixel(argb, x, y);
			}
		}
	}

	int getWidth() { return width; }
	int getHeight() { return height; }
	int *getPixels() { return pixels; }

private:
	int *pixels;
	int width, height;
};

#endif
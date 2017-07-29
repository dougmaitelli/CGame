#include <Windows.h>
#include "GL/gl.h"
#include "GL/glut.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include "PTMReader.h"
#include "GameObject.h"
#include "Timer.h"
#include "Layer.h"

using namespace std;

bool* keyStates = new bool[256];
bool movendo = false;

int alturaPulo = 100;
int velocidade = 10;
int velocidadePulo = 5;

vector<Layer*> layers(5);

Image *personagemSpr;
Image *inimigoSpr;
Image *nuvem, *montanha, *chao, *plataforma, *plataforma2;

Image *scene, *backup;
int *zbuffer, *zbuffer2;

Timer timer;

GameObject *personagem;
int frameCounter = 0;

vector<GameObject*> inimigos;

void preencheZBuffer(int *zbuffer, int z) {
	for (int y = 0; y < scene->getHeight(); y++)
		for (int x = 0; x < scene->getWidth(); x++)
			zbuffer[x + y*scene->getWidth()] = z;
}

void copiaZBuffer(int *zbuffer, int *zbuffer2) {
	for (int y = 0; y < scene->getHeight(); y++)
		for (int x = 0; x < scene->getWidth(); x++)
			zbuffer2[x + y*scene->getWidth()] = zbuffer[x + y*scene->getWidth()];
}

int getChaoY(Layer* layer, int x, int layerOffset) {
	int chaoY = 0;

	bool inicioChao = false;
	for (int i = 0; i < layer->getImagem()->getHeight(); i++) {
		int pixel = layer->getImagem()->getPixel((((int) layer->getPosX() + x) % layer->getImagem()->getWidth()), i);

		if ((pixel >> 24) != 0) {
			inicioChao = true;
			chaoY = i;
		}
		else if (inicioChao) {
			break;
		}
	}

	if (chaoY > 0) {
		chaoY += layerOffset;
	}

	return chaoY;
}

int getChaoY(int x, int y) {
	int chaoYC = getChaoY(layers[4], x, 0);
	int chaoYP1 = getChaoY(layers[3], x, 50);
	int chaoYP2 = getChaoY(layers[2], x, 50);
	
	int chaoY = 0;
	if (chaoYC <= y + 7) {
		chaoY = chaoYC;
	}
	if (chaoYC < chaoYP1 && chaoYP1 <= y + 5) {
		chaoY = chaoYP1;
	}
	if (chaoYP1 < chaoYP2 && chaoYP2 <= y + 5) {
		chaoY = chaoYP2;
	}
	
	return chaoY;
}

void sceneComposition(void) {
	(*scene).plotLayerRepeat(layers[0]->getImagem(), 0, 0, layers[0]->getPosX(), 0);
	preencheZBuffer(zbuffer, 0);

	(*scene).plotLayerZBufferRepeat(layers[1]->getImagem(), 0, 50, layers[1]->getPosX(), 0, zbuffer, 1);
	(*scene).plotLayerZBufferRepeat(layers[2]->getImagem(), 0, 50, layers[2]->getPosX(), 0, zbuffer, 2);
	(*scene).plotLayerZBufferRepeat(layers[3]->getImagem(), 0, 50, layers[3]->getPosX(), 0, zbuffer, 3);
	(*scene).plotLayerZBufferRepeat(layers[4]->getImagem(), 0, 0, layers[4]->getPosX(), 0, zbuffer, 4);

	(*scene).plot(personagem->getCurrentFrame(), personagem->getX(), personagem->getY());

	for (int i = 0; i < inimigos.size(); i++) {
		(*scene).plot(inimigos[i]->getCurrentFrame(), inimigos[i]->getX(), inimigos[i]->getY());
	}

	(*scene).copy(backup);
	copiaZBuffer(zbuffer, zbuffer2);
}

void scroll(bool plus, int valor) {
	for (int i = 0; i < layers.size(); i++) {
		layers[i]->scroll(plus, valor);
	}
}

int calcWaitingTime(int fps, int elapsedTime) {
	int wt = (1000 / fps) - elapsedTime;
	return wt;
}

void keyPressed(unsigned char key, int x, int y) {
	if (key == 'q') {
		return exit(0);
	}

	keyStates[key] = true;

	if (keyStates[32] && personagem->getEstaPulando() == 0) {
		personagem->setEstaPulando(alturaPulo);
	}
	else if (keyStates['s']) {
		personagem->setY(personagem->getY() - 15);
	}
}

void keyUp(unsigned char key, int x, int y) {
	keyStates[key] = false;
}

void keyboardHoldActions() {
	if (keyStates['a']) {
		movendo = true;
		personagem->setDirection(-1);
	}
	else if (keyStates['d']) {
		movendo = true;
		personagem->setDirection(1);
	}
}

int posSinceLastEnemy = 0;
void geraInimigos() {
	if (layers[4]->getPosX() - posSinceLastEnemy > 100 && inimigos.size() < 5) {
		srand(time(NULL));
		if (rand() % 100 > 10) {
			return;
		}

		GameObject* inimigo = new GameObject();
		inimigo->init(PTMReader::read("Graphics/Personagem.ptm"));
		inimigo->setX(600);
		inimigo->setY(300);
		inimigo->setDirection(-1);

		inimigos.push_back(inimigo);

		posSinceLastEnemy = (int)layers[4]->getPosX();
	}
}

void moveInimigos() {
	for (int i = 0; i < inimigos.size(); i++) {
		inimigos[i]->setX(inimigos[i]->getX() - 2);
		if (frameCounter >= 5) {
			inimigos[i]->incCurrentFrame();
		}
	}
}

void calculaGravidade(GameObject* obj) {
	if (obj->getEstaPulando() > 0) {
		obj->setEstaPulando(personagem->getEstaPulando() - velocidadePulo);
		obj->setY(personagem->getY() + velocidadePulo);
	}
	else if (obj->getY() > 0) {
		int chaoYS = getChaoY(obj->getX() + 10, obj->getY());
		int chaoYE = getChaoY(obj->getX() + 60, obj->getY());

		int maxChao = max(chaoYS, chaoYE);

		if (obj->getY() > maxChao) {
			obj->setY(obj->getY() - velocidadePulo);
		}

		if (obj->getY() < maxChao) {
			obj->setY(maxChao);
		}

		if (obj->getY() < 50) {
			obj->setY(50);
		}
	}
}

void update(int value) {
	timer.start();

	keyboardHoldActions();

	int valor = 0;

	if (movendo) {
		valor = personagem->getDirection() * velocidade;
	}

	if (movendo && frameCounter >= 5) {
		personagem->incCurrentFrame();
		frameCounter = 0;
	}
	frameCounter++;

	calculaGravidade(personagem);
	for (int i = 0; i < inimigos.size(); i++) {
		calculaGravidade(inimigos[i]);
	}

	scroll(false, valor);

	geraInimigos();
	moveInimigos();

	sceneComposition();
	glutPostRedisplay();

	timer.finish();

	movendo = false;

	int waitingTime = calcWaitingTime(60, timer.getElapsedTimeMs());
	glutTimerFunc(waitingTime, update, value);
}

void display(void) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

	GLint iViewport[4];
	glGetIntegerv(GL_VIEWPORT, iViewport);
	glPixelZoom((float) iViewport[2] / (float) (*scene).getWidth(), (float) iViewport[3] / (float) (*scene).getHeight());

	glDrawPixels((*scene).getWidth(), (*scene).getHeight(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, (*scene).getPixels());
	glutSwapBuffers();

	glFlush();
}

void initJogo(void) {
	scene = new Image(640, 480);
	backup = new Image(640, 480);

	zbuffer = new int[scene->getWidth()*scene->getHeight()];
	zbuffer2 = new int[scene->getWidth()*scene->getHeight()];

	nuvem = PTMReader::read("Graphics/Nuvens.ptm");
	montanha = PTMReader::read("Graphics/Montanhas.ptm");
	plataforma2 = PTMReader::read("Graphics/Plataforma2.ptm");
	plataforma = PTMReader::read("Graphics/Plataforma.ptm");
	chao = PTMReader::read("Graphics/Chao.ptm");

	for(int i = 0; i < layers.size(); i++)
		layers[i] = new Layer();

	layers[0]->setImagem(nuvem);
	layers[1]->setImagem(montanha);
	layers[2]->setImagem(plataforma2);
	layers[3]->setImagem(plataforma);
	layers[4]->setImagem(chao);

	float mainWidth = layers[4]->getImagem()->getWidth();

	for(int i = 0; i < layers.size(); i++)
		layers[i]->setTaxaX(layers[i]->getImagem()->getWidth() / mainWidth);
}

void initPersonagem(void) {
	personagem = new GameObject();
	personagem->init(PTMReader::read("Graphics/Personagem.ptm"));
	personagem->setX(150);
	personagem->setY(300);
}

void reshape(int width, int height) {
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Exercicio Parallax");

	initJogo();
	initPersonagem();
	sceneComposition();
	update(0);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	for (int i = 0; i < 255; i++)
		keyStates[i] = false;

	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyUp);

	glutMainLoop();
	return 0;
}

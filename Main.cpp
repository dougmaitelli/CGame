#include <iostream>
#include <algorithm>
#include <math.h>

#include "include/GLFW/glfw3.h"

#include "PTMReader.h"
#include "GameObject.h"
#include "Timer.h"
#include "Layer.h"

using namespace std;

bool moving = false;

int alturaPulo = 100;
int velocity = 10;
int jumpVelocity = 5;

vector<Layer*> layers(5);

Image *playerSpr;
Image *enemySpr;
Image *clouds, *mountain, *ground, *platform, *backPlatform;

Image *scene;
int *zbuffer;

Timer timer;

GameObject *player;
int frameCounter = 0;

vector<GameObject*> enemies;

void clearZBuffer(int *zbuffer) {
	for (int y = 0; y < scene->getHeight(); y++)
		for (int x = 0; x < scene->getWidth(); x++)
			zbuffer[x + y*scene->getWidth()] = 0;
}

int getGroundY(Layer* layer, int x, int layerOffset) {
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

int getGroundY(int x, int y) {
	int groundYG = getGroundY(layers[4], x, 0);
	int groundYP = getGroundY(layers[3], x, 50);
	int groundYBP = getGroundY(layers[2], x, 50);
	
	int groundY = 0;
	if (groundYG <= y + 7) {
		groundY = groundYG;
	}
	if (groundYG < groundYP && groundYP <= y + 5) {
		groundY = groundYP;
	}
	if (groundYP < groundYBP && groundYBP <= y + 5) {
		groundY = groundYBP;
	}
	
	return groundY;
}

void sceneComposition(void) {
	(*scene).plotLayerRepeat(layers[0]->getImagem(), 0, 0, layers[0]->getPosX(), 0);
	clearZBuffer(zbuffer);

	(*scene).plotLayerZBufferRepeat(layers[1]->getImagem(), 0, 50, layers[1]->getPosX(), 0, zbuffer, 1);
	(*scene).plotLayerZBufferRepeat(layers[2]->getImagem(), 0, 50, layers[2]->getPosX(), 0, zbuffer, 2);
	(*scene).plotLayerZBufferRepeat(layers[3]->getImagem(), 0, 50, layers[3]->getPosX(), 0, zbuffer, 3);
	(*scene).plotLayerZBufferRepeat(layers[4]->getImagem(), 0, 0, layers[4]->getPosX(), 0, zbuffer, 4);

	(*scene).plot(player->getCurrentFrame(), player->getX(), player->getY());

	for (int i = 0; i < enemies.size(); i++) {
		(*scene).plot(enemies[i]->getCurrentFrame(), enemies[i]->getX(), enemies[i]->getY());
	}
}

void scroll(bool plus, int valor) {
	for (int i = 0; i < layers.size(); i++) {
		layers[i]->scroll(plus, valor);
	}
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if(key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		if (key == GLFW_KEY_SPACE && player->getEstaPulando() == 0) {
			player->setEstaPulando(alturaPulo);
		}
		else if (key == GLFW_KEY_S) {
			player->setY(player->getY() - 15);
		}

		if (key == GLFW_KEY_A) {
			moving = true;
			player->setDirection(-1);
		}
		else if (key == GLFW_KEY_D) {
			moving = true;
			player->setDirection(1);
		}
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A || key == GLFW_KEY_D) {
			moving = false;
		}
	}
}

int posSinceLastEnemy = 0;
void generateEnemies() {
	if (layers[4]->getPosX() - posSinceLastEnemy > 100 && enemies.size() < 5) {
		srand(time(NULL));
		if (rand() % 100 > 10) {
			return;
		}

		GameObject* enemy = new GameObject();
		enemy->init(PTMReader::read("Graphics/Personagem.ptm"));
		enemy->setX(600);
		enemy->setY(300);
		enemy->setDirection(-1);

		enemies.push_back(enemy);

		posSinceLastEnemy = (int)layers[4]->getPosX();
	}
}

void moveEnemies() {
	for (int i = 0; i < enemies.size(); i++) {
		enemies[i]->setX(enemies[i]->getX() - 2);
		if (frameCounter >= 5) {
			enemies[i]->incCurrentFrame();
		}
	}
}

void calcGravity(GameObject* obj) {
	if (obj->getEstaPulando() > 0) {
		obj->setEstaPulando(player->getEstaPulando() - jumpVelocity);
		obj->setY(player->getY() + jumpVelocity);
	}
	else if (obj->getY() > 0) {
		int chaoYS = getGroundY(obj->getX() + 10, obj->getY());
		int chaoYE = getGroundY(obj->getX() + 60, obj->getY());

		int maxGround = max(chaoYS, chaoYE);

		if (obj->getY() > maxGround) {
			obj->setY(obj->getY() - jumpVelocity);
		}

		if (obj->getY() < maxGround) {
			obj->setY(maxGround);
		}

		if (obj->getY() < 50) {
			obj->setY(50);
		}
	}
}

void update() {
	int value = 0;

	if (moving) {
		value = player->getDirection() * velocity;
	}

	if (moving && frameCounter >= 5) {
		player->incCurrentFrame();
		frameCounter = 0;
	}
	frameCounter++;

	calcGravity(player);
	for (int i = 0; i < enemies.size(); i++) {
		calcGravity(enemies[i]);
	}

	scroll(false, value);

	generateEnemies();
	moveEnemies();

	sceneComposition();
}

void initJogo(void) {
	scene = new Image(640, 480);

	zbuffer = new int[scene->getWidth()*scene->getHeight()];

	clouds = PTMReader::read("Graphics/Nuvens.ptm");
	mountain = PTMReader::read("Graphics/Montanhas.ptm");
	backPlatform = PTMReader::read("Graphics/Plataforma2.ptm");
	platform = PTMReader::read("Graphics/Plataforma.ptm");
	ground = PTMReader::read("Graphics/Chao.ptm");

	for(int i = 0; i < layers.size(); i++)
		layers[i] = new Layer();

	layers[0]->setImagem(clouds);
	layers[1]->setImagem(mountain);
	layers[2]->setImagem(backPlatform);
	layers[3]->setImagem(platform);
	layers[4]->setImagem(ground);

	float mainWidth = layers[4]->getImagem()->getWidth();

	for(int i = 0; i < layers.size(); i++)
		layers[i]->setTaxaX(layers[i]->getImagem()->getWidth() / mainWidth);
}

void initPersonagem(void) {
	player = new GameObject();
	player->init(PTMReader::read("Graphics/Personagem.ptm"));
	player->setX(150);
	player->setY(300);
}

int main(int argc, char** argv) {
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Game 2D", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	initJogo();
	initPersonagem();
	sceneComposition();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);

		update();

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawPixels((*scene).getWidth(), (*scene).getHeight(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, (*scene).getPixels());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	return EXIT_SUCCESS;
}

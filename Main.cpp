#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <math.h>

#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"

#include "src/Graphics/GraphicReader.h"
#include "src/Player.h"
#include "src/Enemy.h"
#include "src/Layer.h"

#define GAME_WIDTH 640
#define GAME_HEIGHT 480

using namespace std;

int jumpStrength = 100;
int velocity = 350;
int jumpVelocity = 5;

vector<Layer*> layers(5);

Image *playerSpr;
Image *enemySpr;
Image *clouds, *mountain, *ground, *platform, *backPlatform;

Image *scene;

Player *player;

vector<Enemy*> enemies;

void clearZBuffer(int *zbuffer) {
	for (int y = 0; y < scene->getHeight(); y++)
		for (int x = 0; x < scene->getWidth(); x++)
			zbuffer[x + y * scene->getWidth()] = 0;
}

int getGroundY(Layer* layer, int x, int layerOffset) {
	int groundY = 0;

	bool groundStart = false;
	for (int i = 0; i < layer->getImage()->getHeight(); i++) {
		int pixel = layer->getImage()->getPixel((((int) layer->getPosX() + x) % layer->getImage()->getWidth()), i);

		if ((pixel >> 24) != 0) {
			groundStart = true;
			groundY = i;
		}
		else if (groundStart) {
			break;
		}
	}

	if (groundY > 0) {
		groundY += layerOffset;
	}

	return groundY;
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

void sceneComposition() {
	(*scene).plotLayerRepeat(layers[0]->getImage(), 0, 0, layers[0]->getPosX(), 0);

	(*scene).plotLayerRepeat(layers[1]->getImage(), 0, 50, layers[1]->getPosX(), 0);
	(*scene).plotLayerRepeat(layers[2]->getImage(), 0, 50, layers[2]->getPosX(), 0);
	(*scene).plotLayerRepeat(layers[3]->getImage(), 0, 50, layers[3]->getPosX(), 0);
	(*scene).plotLayerRepeat(layers[4]->getImage(), 0, 0, layers[4]->getPosX(), 0);

	(*scene).plot(player->getCurrentFrame(), player->getX(), player->getY());

	for (unsigned int i = 0; i < enemies.size(); i++) {
		(*scene).plot(enemies[i]->getCurrentFrame(), enemies[i]->getX(), enemies[i]->getY());
	}
}

void scroll(bool plus, int value) {
	for (unsigned int i = 0; i < layers.size(); i++) {
		layers[i]->scroll(plus, value);
	}
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if(key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		if (key == GLFW_KEY_SPACE && player->getJumping() == 0 && !player->isFalling()) {
			player->setJumping(jumpStrength);
		}
		else if (key == GLFW_KEY_S) {
			player->setY(player->getY() - 15);
		}

		if (key == GLFW_KEY_A) {
			player->setMoving(true);
			player->setDirection(-1);
		}
		else if (key == GLFW_KEY_D) {
			player->setMoving(true);
			player->setDirection(1);
		}
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A || key == GLFW_KEY_D) {
			player->setMoving(false);
		}
	}
}

std::default_random_engine generator;
std::uniform_int_distribution<int> distribution(1, 500);

int posSinceLastEnemy = 0;
void generateEnemies() {
	if (layers[4]->getPosX() - posSinceLastEnemy > 100 && enemies.size() < 5) {
		int dice_roll = distribution(generator);

		if (dice_roll != 1) {
			return;
		}

		Enemy* enemy = new Enemy();
		enemy->init(GraphicReader::readPtm("Graphics/Enemy.ptm"));
		enemy->setX(600);
		enemy->setY(300);
		enemy->setDirection(-1);

		enemies.push_back(enemy);

		posSinceLastEnemy = (int)layers[4]->getPosX();
	}
}

void moveEnemies(int movingOffeset) {
	vector<int> enemiesToRemove;

	for (unsigned int i = 0; i < enemies.size(); i++) {
		enemies[i]->setX(enemies[i]->getX() - 2 - movingOffeset);
		enemies[i]->incCurrentFrame();

		if (enemies[i]->getX() < enemies[i]->getCurrentFrame()->getWidth() * -1) {
			enemiesToRemove.push_back(i);
		}
	}

	for (unsigned int i = enemiesToRemove.size(); i > 0; i--) {
		enemies.erase(enemies.begin() + enemiesToRemove[i]);
	}
}

void calcGravity(GameObject* obj) {
	if (obj->getJumping() > 0) {
		obj->setJumping(player->getJumping() - jumpVelocity);
		obj->setY(player->getY() + jumpVelocity);
	} else if (obj->getY() > 0) {
		int chaoYS = getGroundY(obj->getX() + 10, obj->getY());
		int chaoYE = getGroundY(obj->getX() + 60, obj->getY());

		int maxGround = max(chaoYS, chaoYE);

		if (obj->getY() > maxGround + 50) {
			obj->setFalling(true);
		}

		if (obj->getY() > maxGround) {
			obj->setY(obj->getY() - jumpVelocity);
		}

		if (obj->getY() <= maxGround && obj->isFalling()) {
			obj->setFalling(false);
		}

		if (obj->getY() < maxGround) {
			obj->setY(maxGround);
		}

		if (obj->getY() < 50) {
			obj->setY(50);
		}
	}
}

double lastTime = 0;
void update() {
	double time = glfwGetTime();
	double elapsedSeconds = time - lastTime;

	int movingValue = 0;

	if (player->isMoving()) {
		movingValue = player->getDirection() * (int) ((double) velocity * elapsedSeconds);
	} else {
		lastTime = time;
	}

	player->incCurrentFrame();

	calcGravity(player);
	for (unsigned int i = 0; i < enemies.size(); i++) {
		calcGravity(enemies[i]);
	}

	if (movingValue != 0) {
		scroll(false, movingValue);
		lastTime = time;
	}

	generateEnemies();
	moveEnemies(movingValue);
}

void initJogo() {
	scene = new Image(GAME_WIDTH, GAME_HEIGHT);

	clouds = GraphicReader::readPtm("Graphics/Clouds.ptm");
	mountain = GraphicReader::readPtm("Graphics/Mountain.ptm");
	backPlatform = GraphicReader::readPtm("Graphics/BackPlatform.ptm");
	platform = GraphicReader::readPtm("Graphics/Platform.ptm");
	ground = GraphicReader::readImage("Graphics/ground.png");

	for (unsigned int i = 0; i < layers.size(); i++)
		layers[i] = new Layer();

	layers[0]->setImagem(clouds);
	layers[1]->setImagem(mountain);
	layers[2]->setImagem(backPlatform);
	layers[3]->setImagem(platform);
	layers[4]->setImagem(ground);

	float mainWidth = layers[4]->getImage()->getWidth();

	for (unsigned int i = 0; i < layers.size(); i++)
		layers[i]->setTaxaX(layers[i]->getImage()->getWidth() / mainWidth);
}

void initPersonagem(void) {
	player = new Player();
	player->init(GraphicReader::readImage("Graphics/player.png"));
	player->setX(150);
	player->setY(300);
}

int main(int argc, char** argv) {
	GLFWwindow* window;

	GLuint m_buffer, program;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	ilInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(GAME_WIDTH, GAME_HEIGHT, "Game 2D", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	/* Listen to key input */
	glfwSetKeyCallback(window, key_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval(1);

	initJogo();
	initPersonagem();

	program = glCreateProgram();
	glLinkProgram(program);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);

		/* Render here */
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		update();
		sceneComposition();

		glUseProgram(program);
		glDrawPixels((*scene).getWidth(), (*scene).getHeight(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, (*scene).getPixels());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <math.h>

#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"

#include "src/Graphics/GraphicReader.h"
#include "src/Player.h"
#include "src/Shot.h"
#include "src/Enemy.h"
#include "src/Layer.h"

#define GAME_WIDTH 640
#define GAME_HEIGHT 480

using namespace std;

unsigned int playerMaxMarginFromX = 150;
int jumpStrength = 100;
int velocity = 350;
int jumpVelocity = 5;

vector<Layer*> layers(4);

Image *scene;

Player *player;
vector<Shot*> shots;

Image* enemySpr;
vector<Enemy*> enemies;

int getGroundY(Layer* layer, int x, int layerOffset) {
	if (x < 0) {
		return 0;
	}

	int groundY = 0;

	bool groundStart = false;
	for (int i = 0; i < layer->getImage()->getHeight(); i++) {
		int pixel = layer->getImage()->getPixel((((int) layer->getPosX() + x) % layer->getImage()->getWidth()), i);

		if ((pixel >> 24) != 0) {
			groundStart = true;
			groundY = i;
		} else if (groundStart) {
			break;
		}
	}

	if (groundY > 0) {
		groundY += layerOffset;
	}

	return groundY;
}

int getHigherGroundY(int x, int y) {
	int groundYG = getGroundY(layers[3], x, 0);
	int groundYP = getGroundY(layers[2], x, 64);
	int groundYBP = getGroundY(layers[1], x, 64);
	
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

int getLowerGroundY(int x) {
	return getGroundY(layers[3], x, 0);
}


void sceneComposition() {
	(*scene).fill(85, 170, 255);

	(*scene).plotLayerRepeat(layers[0]->getImage(), 0, 0, layers[0]->getPosX(), 0);
	(*scene).plotLayerRepeat(layers[1]->getImage(), 0, 64, layers[1]->getPosX(), 0);
	(*scene).plotLayerRepeat(layers[2]->getImage(), 0, 64, layers[2]->getPosX(), 0);
	(*scene).plotLayerRepeat(layers[3]->getImage(), 0, 0, layers[3]->getPosX(), 0);

	(*scene).plot(player->getCurrentFrame(), player->getX(), player->getY());

	for (unsigned int i = 0; i < enemies.size(); i++) {
		(*scene).plot(enemies[i]->getCurrentFrame(), enemies[i]->getX(), enemies[i]->getY());
	}

	for (unsigned int i = 0; i < shots.size(); i++) {
		(*scene).plot(shots[i]->getCurrentFrame(), shots[i]->getX(), shots[i]->getY());
	}
}

void scroll(bool plus, int value) {
	for (unsigned int i = 0; i < layers.size(); i++) {
		layers[i]->scroll(plus, value);
	}
}

void shot() {
	Shot* shot = new Shot();
	shot->init(GraphicReader::readImage("Graphics/shot.png"));
	shot->setX(player->getX() + 50);
	shot->setY(player->getY());
	shot->setDirection(1);

	shots.push_back(shot);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if(key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		if (player->getJumping() == 0 && !player->isFalling()) {
			if (key == GLFW_KEY_SPACE) {
				player->setJumping(jumpStrength);
			} else if (key == GLFW_KEY_S && player->getY() > getLowerGroundY(player->getX())) {
				player->setY(player->getY() - 15);
			}

			if (key == GLFW_KEY_A) {
				player->setMoving(true);
				player->setDirection(-1);
			} else if (key == GLFW_KEY_D) {
				player->setMoving(true);
				player->setDirection(1);
			}
		}

		if (key == GLFW_KEY_LEFT_CONTROL) {
			player->startShooting();
			shot();
		}
	} else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A || key == GLFW_KEY_D) {
			player->setMoving(false);
		}

		if (key == GLFW_KEY_LEFT_CONTROL) {
			player->stopShooting();
		}
	}
}

std::default_random_engine generator;
std::uniform_int_distribution<int> distribution(1, 500);

int posSinceLastEnemy = 0;
void generateEnemies() {
	if (layers[3]->getPosX() - posSinceLastEnemy > 100 && enemies.size() < 5) {
		int dice_roll = distribution(generator);

		if (dice_roll != 1) {
			return;
		}

		Enemy* enemy = new Enemy();
		enemy->init(enemySpr);
		enemy->setX(800);
		enemy->setY(300);
		enemy->setDirection(-1);

		enemies.push_back(enemy);

		posSinceLastEnemy = (int)layers[3]->getPosX();
	}
}

void moveEnemies(int movingOffeset) {
	vector<int> enemiesToRemove;

	for (unsigned int i = 0; i < enemies.size(); i++) {
		enemies[i]->setX(enemies[i]->getX() - 2 - movingOffeset);

		if (enemies[i]->getX() < enemies[i]->getCurrentFrame()->getWidth() * -1) {
			enemiesToRemove.push_back(i);
		}
	}

	for (unsigned int i = enemiesToRemove.size(); i > 0; i--) {
		enemies.erase(enemies.begin() + enemiesToRemove[i - 1]);
	}
}

void moveShots(int movingOffeset) {
	vector<int> shotsToRemove;

	for (unsigned int i = 0; i < shots.size(); i++) {
		shots[i]->setX(shots[i]->getX() + 7 - movingOffeset);

		if (shots[i]->getX() < shots[i]->getCurrentFrame()->getWidth() * -1 || shots[i]->getX() > GAME_WIDTH) {
			shotsToRemove.push_back(i);
		}
	}

	for (unsigned int i = shotsToRemove.size(); i > 0; i--) {
		shots.erase(shots.begin() + shotsToRemove[i - 1]);
	}
}

void calcGravity(GameObject* obj) {
	if (obj->getJumping() > 0) {
		obj->setJumping(obj->getJumping() - jumpVelocity);
		obj->setY(obj->getY() + jumpVelocity);
	} else if (obj->getY() > 0) {
		int groundYS = getHigherGroundY(obj->getX() + 10, obj->getY());
		int groundYE = getHigherGroundY(obj->getX() + 60, obj->getY());

		int maxGround = max(groundYS, groundYE);

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

	calcGravity(player);
	for (unsigned int i = 0; i < enemies.size(); i++) {
		calcGravity(enemies[i]);
	}

	if (movingValue != 0) {
		if (layers[3]->getPosX() == 0 && movingValue < 0 || player->getX() < playerMaxMarginFromX && movingValue > 0) {
			player->setX(player->getX() + movingValue);

			if (player->getX() < 0) {
				player->setX(0);
			}

			movingValue = 0;
		} else {
			scroll(false, movingValue);
		}

		lastTime = time;
	}

	moveShots(movingValue);

	generateEnemies();
	moveEnemies(movingValue);
}

void initJogo() {
	scene = new Image(GAME_WIDTH, GAME_HEIGHT);

	Image *clouds, *ground, *platform, *backPlatform;
	clouds = GraphicReader::readImage("Graphics/clouds.png");
	backPlatform = GraphicReader::readImage("Graphics/backPlatform.png");
	platform = GraphicReader::readImage("Graphics/platform.png");
	ground = GraphicReader::readImage("Graphics/ground.png");

	layers[0] = new Layer(clouds, 3);
	layers[1] = new Layer(backPlatform, 2);
	layers[2] = new Layer(platform, 1);
	layers[3] = new Layer(ground, 0);

	enemySpr = GraphicReader::readImage("Graphics/enemy.png");
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

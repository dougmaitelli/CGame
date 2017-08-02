#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Graphics/Image.h"
#include "Graphics/Animation.h"

class GameObject {
public:
	void init(Image* sprite) {
		this->sprite = sprite;

		initAnimations(sprite);
	}

	virtual void initAnimations(Image* sprite) = 0;
	virtual Animation* getCurrentAnimation() = 0;

	Image* getCurrentFrame() {
		return (*getCurrentAnimation()).getFrame(currentFrame);
	}

	double lastTime = 0;
	void incCurrentFrame() {
		double time = glfwGetTime();
		double elapsedSeconds = time - lastTime;

		if (elapsedSeconds > 0.1) {
			currentFrame = (currentFrame + 1) % (*getCurrentAnimation()).getNum();
			lastTime = time;
		}
	}

	int getX(void) {
		return posX;
	}

	int getY(void) {
		return posY;
	}

	void setX(int x) {
		posX = x;
	}

	void setY(int y) {
		posY = y;
	}

	int getDirection() {
		return direction;
	}

	void setDirection(int direction) {
		this->direction = direction;
	}

	bool isMoving() {
		return moving;
	}

	void setMoving(bool moving) {
		this->moving = moving;
		currentFrame = 0;
	}

	int getJumping() {
		return jumping;
	}

	void setJumping(int jumping) {
		this->jumping = jumping;
		currentFrame = 0;
	}

	bool isFalling() {
		return falling;
	}

	void setFalling(bool falling) {
		this->falling = falling;
		currentFrame = 0;
	}

	void startShooting() {
		if (!moving) {
			currentFrame = 0;
		}

		shooting = true;
	}

	void stopShooting() {
		shooting = false;
	}

	bool isShooting() {
		return shooting;
	}

private:
	Image* sprite;
	int currentFrame = 0;

	int posX, posY;
	int direction = 0;

	bool moving = false;
	int jumping = 0;
	bool falling = false;
	bool shooting = false;
};

#endif
#ifndef GAME_ANIMATION_H
#define GAME_ANIMATION_H

#include <vector>

#include "Image.h"

using namespace std;

class Animation {
public:
	Animation() {}

	Animation(unsigned int numFrames, int width, int height) {
		for (int i = 0; i < numFrames; i++) {
			frames.push_back(new Image(width, height));
		}
		num = frames.size();
	}

	Image* getFrame(unsigned int number) {
		return frames.at(number);
	}

	unsigned int getNum() {
		return num;
	}

	unsigned int getLoopFrom() {
		return loopFrom;
	}

	void setLoopFrom(unsigned int loopFrom) {
		this->loopFrom = loopFrom;
	}

	Image* getCurrentFrame() {
		double time = glfwGetTime();
		double elapsedSeconds = time - lastTime;

		if (elapsedSeconds > 0.1) {
			currentFrame++;

			if (currentFrame >= num) {
				currentFrame = loopFrom;
			}

			lastTime = time;
		}

		return getFrame(currentFrame);
	}

private:
	vector<Image*> frames;
	unsigned int num = 0;
	unsigned int loopFrom = 0;

	unsigned int currentFrame = 0;
	double lastTime = 0;
};

#endif
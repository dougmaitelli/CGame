#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include "GameObject.h"
#include "Graphics/Image.h"
#include "Graphics/Animation.h"

class Enemy : public GameObject {
public:
	void initAnimations(Image* sprite) {
		animRun = new Animation(4, (*sprite).getWidth() / 4, (*sprite).getHeight() / 4);
		(*sprite).subImage(animRun->getFrame(0), 0, 128);
		(*sprite).subImage(animRun->getFrame(1), 64, 128);
		(*sprite).subImage(animRun->getFrame(2), 128, 128);
		(*sprite).subImage(animRun->getFrame(3), 192, 128);
	}

	Animation* getCurrentAnimation() {
		return animRun;
	}

private:
	Animation* animRun;
};

#endif
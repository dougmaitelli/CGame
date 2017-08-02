#ifndef GAME_ENEMY_H
#define GAME_ENEMY_H

#include "GameObject.h"
#include "Graphics/Image.h"
#include "Graphics/Animation.h"

class Enemy : public GameObject {
public:
	void initAnimations(Image* sprite) {
		animRunBack = new Animation(6, 76, 96);
		(*sprite).subImageMirrored(animRunBack->getFrame(0), 1, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImageMirrored(animRunBack->getFrame(1), 78, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImageMirrored(animRunBack->getFrame(2), 155, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImageMirrored(animRunBack->getFrame(3), 232, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImageMirrored(animRunBack->getFrame(4), 309, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImageMirrored(animRunBack->getFrame(5), 386, (*sprite).getHeight() - 100 - 96 - 1);
	}

	Animation* getCurrentAnimation() {
		return animRunBack;
	}

private:
	Animation* animRunBack;
};

#endif
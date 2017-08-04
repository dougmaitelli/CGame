#ifndef GAME_SHOT_H
#define GAME_SHOT_H

#include "Graphics/Image.h"
#include "Graphics/Animation.h"

class Shot : public GameObject {
public:
	void initAnimations(Image* sprite) {
		animRun = new Animation(7, 76, 96);
		(*sprite).subImage(animRun->getFrame(0), 1, (*sprite).getHeight() - 1 - 96 - 1);
		(*sprite).subImage(animRun->getFrame(1), 78, (*sprite).getHeight() - 1 - 96 - 1);
		(*sprite).subImage(animRun->getFrame(2), 155, (*sprite).getHeight() - 1 - 96 - 1);
		(*sprite).subImage(animRun->getFrame(3), 232, (*sprite).getHeight() - 1 - 96 - 1);
		(*sprite).subImage(animRun->getFrame(4), 309, (*sprite).getHeight() - 1 - 96 - 1);
		(*sprite).subImage(animRun->getFrame(5), 386, (*sprite).getHeight() - 1 - 96 - 1);
		(*sprite).subImage(animRun->getFrame(6), 463, (*sprite).getHeight() - 1 - 96 - 1);
	}

	Animation* getCurrentAnimation() {
		return animRun;
	}

private:
	Animation* animRun;
};

#endif
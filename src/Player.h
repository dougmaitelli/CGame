#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "GameObject.h"
#include "Graphics/Image.h"
#include "Graphics/Animation.h"

class Player : public GameObject {
public:
	void initAnimations(Image* sprite) {
		animStand = new Animation(2, 76, 96);
		(*sprite).subImage(animStand->getFrame(0), 1, (*sprite).getHeight() - 1 - 96 - 1);
		(*sprite).subImage(animStand->getFrame(1), 78, (*sprite).getHeight() - 1 - 96 - 1);

		animRun = new Animation(6, 76, 96);
		(*sprite).subImage(animRun->getFrame(0), 1, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImage(animRun->getFrame(1), 78, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImage(animRun->getFrame(2), 155, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImage(animRun->getFrame(3), 232, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImage(animRun->getFrame(4), 309, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImage(animRun->getFrame(5), 386, (*sprite).getHeight() - 100 - 96 - 1);

		animJump = new Animation(1, 76, 96);
		(*sprite).subImage(animJump->getFrame(0), 309, (*sprite).getHeight() - 1 - 96 - 1);

		animFall = new Animation(1, 76, 96);
		(*sprite).subImage(animFall->getFrame(0), 386, (*sprite).getHeight() - 1 - 96 - 1);
	}

	Animation* getCurrentAnimation() {
		if (getJumping() > 0) {
			return animJump;
		} else if (isFalling()) {
			return animFall;
		} else if (isMoving()) {
			return animRun;
		} else {
			return animStand;
		}
	}

private:
	Animation* animStand;
	Animation* animRun;
	Animation* animJump;
	Animation* animFall;
};

#endif
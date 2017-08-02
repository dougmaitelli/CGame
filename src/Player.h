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

		animRunFront = new Animation(6, 76, 96);
		(*sprite).subImage(animRunFront->getFrame(0), 1, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImage(animRunFront->getFrame(1), 78, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImage(animRunFront->getFrame(2), 155, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImage(animRunFront->getFrame(3), 232, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImage(animRunFront->getFrame(4), 309, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImage(animRunFront->getFrame(5), 386, (*sprite).getHeight() - 100 - 96 - 1);

		animRunBack = new Animation(6, 76, 96);
		(*sprite).subImageMirrored(animRunBack->getFrame(0), 1, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImageMirrored(animRunBack->getFrame(1), 78, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImageMirrored(animRunBack->getFrame(2), 155, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImageMirrored(animRunBack->getFrame(3), 232, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImageMirrored(animRunBack->getFrame(4), 309, (*sprite).getHeight() - 100 - 96 - 1);
		(*sprite).subImageMirrored(animRunBack->getFrame(5), 386, (*sprite).getHeight() - 100 - 96 - 1);

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
			if (getDirection() < 0) {
				return animRunBack;
			}

			return animRunFront;
		} else {
			return animStand;
		}
	}

private:
	Animation* animStand;
	Animation* animRunFront;
	Animation* animRunBack;
	Animation* animJump;
	Animation* animFall;
};

#endif
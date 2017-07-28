#include "Animation.h"

class GameObject {
public:
	void init(Image* sprite) {
		this->sprite = sprite;

		anim->push_back(new Animation(4, (*sprite).getWidth() / 4, (*sprite).getHeight() / 4));
		(*sprite).subImage(anim->at(0)->getFrame(0), 0, 192);
		(*sprite).subImage(anim->at(0)->getFrame(1), 64, 192);
		(*sprite).subImage(anim->at(0)->getFrame(2), 128, 192);
		(*sprite).subImage(anim->at(0)->getFrame(3), 192, 192);

		anim->push_back(new Animation(4, (*sprite).getWidth() / 4, (*sprite).getHeight() / 4));
		(*sprite).subImage(anim->at(1)->getFrame(0), 0, 128);
		(*sprite).subImage(anim->at(1)->getFrame(1), 64, 128);
		(*sprite).subImage(anim->at(1)->getFrame(2), 128, 128);
		(*sprite).subImage(anim->at(1)->getFrame(3), 192, 128);

		anim->push_back(new Animation(4, (*sprite).getWidth() / 4, (*sprite).getHeight() / 4));
		(*sprite).subImage(anim->at(2)->getFrame(0), 0, 64);
		(*sprite).subImage(anim->at(2)->getFrame(1), 64, 64);
		(*sprite).subImage(anim->at(2)->getFrame(2), 128, 64);
		(*sprite).subImage(anim->at(2)->getFrame(3), 192, 64);

		anim->push_back(new Animation(4, (*sprite).getWidth() / 4, (*sprite).getHeight() / 4));
		(*sprite).subImage(anim->at(3)->getFrame(0), 0, 0);
		(*sprite).subImage(anim->at(3)->getFrame(1), 64, 0);
		(*sprite).subImage(anim->at(3)->getFrame(2), 128, 0);
		(*sprite).subImage(anim->at(3)->getFrame(3), 192, 0);
	}

	Animation* getCurrentAnimation() {
		return anim->at(estaPulando ? 3 : direction == -1 ? 1 : 2);
	}

	Image* getCurrentFrame() {
		return (*getCurrentAnimation()).getFrame(currentFrame);
	}

	void incCurrentFrame() {
		currentFrame = (currentFrame + 1) % (*getCurrentAnimation()).getNum();
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

	int getEstaPulando() {
		return estaPulando;
	}

	void setEstaPulando(int estaPulando) {
		this->estaPulando = estaPulando;
	}

private:
	Image* sprite;
	vector<Animation*> anim[4];
	int currentFrame = 0;

	int posX, posY;
	int direction = 0;
	int estaPulando = 0;
};
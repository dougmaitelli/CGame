#include <vector>

using namespace std;

class Animation {
public:
	Animation() {}

	Animation(int numFrames, int height, int width) {
		for (int i = 0; i < numFrames; i++) {
			frames.push_back(new Image(width, height));
		}
		num = frames.size();
	}

	Image * getFrame(int number) {
		return frames.at(number);
	}

	int getNum(void) {
		return num;
	}

private:
	vector<Image*> frames;
	int num = 0;
};
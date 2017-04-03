#include "gamescreen.h"

#include <stdlib.h>

#include <tari/input.h>
#include <tari/wrapper.h>
#include <tari/animationtree.h>

static struct {
	int x;

} gData;

static void loadGameScreen() {
	AnimationTree test = loadAnimationTree("assets/player/PLAYERTREE.txt");
	playAnimationTreeLoop(makePosition(0, 0, 1), test, "IDLE");
}

static void unloadGameScreen() {}

static void updateGameScreen() {}

static void drawGameScreen() {}

static Screen* getNextGameScreenScreen() {

	if (hasPressedAbortFlank()) {
		abortScreenHandling();
	}

	return NULL;
}



Screen GameScreen = {
	.mLoad = loadGameScreen,
	.mUnload = unloadGameScreen,
	.mUpdate = updateGameScreen,
	.mDraw = drawGameScreen,
	.mGetNextScreen = getNextGameScreenScreen
};
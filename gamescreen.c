#include "gamescreen.h"

#include <stdlib.h>

#include <tari/input.h>
#include <tari/wrapper.h>
#include <tari/animationtree.h>
#include <tari/stagehandler.h>

#include "player.h"

static void loadGameScreen() {
	setupAnimationTreeHandling();

	loadStageFromScript("assets/level1/stage/STAGE1.txt");

	loadPlayer();
}

static void unloadGameScreen() {
	shutdownAnimationTreeHandling();
}

static void updateGameScreen() {
	updateAnimationTreeHandling();
	
	updatePlayer();
}

static void drawGameScreen() {
	drawAnimationTreeHandling();

}

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
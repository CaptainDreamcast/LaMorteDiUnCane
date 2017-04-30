#include "titlescreen.h"

#include <stdlib.h>

#include <tari/input.h>
#include <tari/animation.h>
#include <tari/file.h>

#include "stage.h"
#include "gamescreen.h"

static struct {
	Animation playerAnimation;
	TextureData playerTextures[10];

	Animation kidnapperAnimation;
	TextureData kidnapperTextures[10];

	TextureData shadowTexture;

	TextureData title;
	TextureData pressStart;

} gData;

static void loadTitleScreen() {
	loadStage();

	gData.shadowTexture = loadTexture("assets/sprites/SHADOW.pkg");

	int i;

	gData.playerAnimation = createEmptyAnimation();
	gData.playerAnimation.mFrameAmount = 6;
	gData.playerAnimation.mDuration = 3;
	for (i = 0; i < (int)gData.playerAnimation.mFrameAmount; i++) {
		char path[1024];
		getPathWithNumberAffixedFromAssetPath(path, "assets/sprites/RIDING.pkg", i);
		gData.playerTextures[i] = loadTexture(path);
	}
	playAnimationLoop(makePosition(100, 350, 5), gData.playerTextures, gData.playerAnimation, makeRectangleFromTexture(gData.playerTextures[0]));
	playAnimationLoop(makePosition(100, 382, 3), &gData.shadowTexture, createOneFrameAnimation(), makeRectangleFromTexture(gData.shadowTexture));


	gData.kidnapperAnimation = createEmptyAnimation();
	gData.kidnapperAnimation.mFrameAmount = 6;
	gData.kidnapperAnimation.mDuration = 4;
	for (i = 0; i < (int)gData.kidnapperAnimation.mFrameAmount; i++) {
		char path[1024];
		getPathWithNumberAffixedFromAssetPath(path, "assets/sprites/KIDNAPPER.pkg", i);
		gData.kidnapperTextures[i] = loadTexture(path);
	}
	playAnimationLoop(makePosition(450, 350, 5), gData.kidnapperTextures, gData.kidnapperAnimation, makeRectangleFromTexture(gData.kidnapperTextures[0]));
	playAnimationLoop(makePosition(450, 382, 3), &gData.shadowTexture, createOneFrameAnimation(), makeRectangleFromTexture(gData.shadowTexture));


	gData.title = loadTexture("assets/sprites/TITLE.pkg");
	playAnimationLoop(makePosition(60, 80, 3), &gData.title, createOneFrameAnimation(), makeRectangleFromTexture(gData.title));

	gData.pressStart = loadTexture("assets/sprites/PRESSSTART.pkg");
	playAnimationLoop(makePosition(60, 150, 3), &gData.pressStart, createOneFrameAnimation(), makeRectangleFromTexture(gData.pressStart));

}


static void unloadTitleScreen() {

}


static void updateTitleScreen() {
	updateStage();
}

static void drawTitleScreen() {

}

static Screen* getNextTitleScreenScreen() {
	if (hasPressedAbortFlank()) {
		abortScreenHandling();
	}

	else if (hasPressedStartFlank()) {
		return &GameScreen;
	}

	return NULL;
}


Screen TitleScreen = {
	.mLoad = loadTitleScreen,
	.mUnload = unloadTitleScreen,
	.mUpdate = updateTitleScreen,
	.mDraw = drawTitleScreen,
	.mGetNextScreen = getNextTitleScreenScreen
};
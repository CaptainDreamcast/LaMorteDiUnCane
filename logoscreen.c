#include "logoscreen.h"

#include <stdlib.h>

#include <tari/input.h>
#include <tari/timer.h>
#include <tari/animation.h>

#include "titlescreen.h"

static struct {
	int mIsOver;

	int animationID;
	TextureData logo;
} gData;

static void setLogoScreenOver(void* caller) {
	gData.mIsOver = 1;
}

static void loadLogoScreen() {

	gData.logo = loadTexture("assets/sprites/LOGO.pkg");
	gData.animationID = playAnimationLoop(makePosition(0,0,1), &gData.logo, createOneFrameAnimation(), makeRectangleFromTexture(gData.logo));
	fadeInAnimation(gData.animationID, 60);

	addTimerCB(260, setLogoScreenOver, NULL);
	gData.mIsOver = 0;
}


static void unloadLogoScreen() {}
static void updateLogoScreen() {}
static void drawLogoScreen() {}
static Screen* getNextLogoScreenScreen() {
	if (hasPressedAbortFlank()) {
		abortScreenHandling();
	}
	
	if (gData.mIsOver || hasPressedStartFlank()) {
		return &TitleScreen;
	}

	return NULL;
}

Screen LogoScreen = {
	.mLoad = loadLogoScreen,
	.mUnload = unloadLogoScreen,
	.mUpdate = updateLogoScreen,
	.mDraw = drawLogoScreen,
	.mGetNextScreen = getNextLogoScreenScreen
};																											 

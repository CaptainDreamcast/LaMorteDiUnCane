#include "retryscreen.h"

#include <stdlib.h>
#include <math.h>

#include <tari/input.h>
#include <tari/animation.h>
#include <tari/file.h>

#include "stage.h"
#include "gamescreen.h"
#include "titlescreen.h"

static struct {
	
	TextureData retry;

	TextureData numbers[10];
	TextureData point;

	int isTimerFinished;

	Position distancePosition;
	double distanceTravelled;
	TextureData miles;
	TextureData distanceTexture;

} gData;

static void timerFinished(void* caller) {
	gData.isTimerFinished = 1;
}

static void loadRetryScreen() {
	gData.distanceTravelled = getDistanceTravelledDuringLastScreen();
	

	loadStage();

	gData.retry = loadTexture("assets/sprites/RETRY.pkg");
	playAnimationLoop(makePosition(60, 210, 3), &gData.retry, createOneFrameAnimation(), makeRectangleFromTexture(gData.retry));

	Animation numberAnimation = createEmptyAnimation();
	numberAnimation.mFrameAmount = 10;
	numberAnimation.mDuration = 90;
	int i;
	for (i = 0; i < (int)numberAnimation.mFrameAmount; i++) {
		char path[1024];
		getPathWithNumberAffixedFromAssetPath(path, "assets/sprites/NUMBERS.pkg", i);
		gData.numbers[i] = loadTexture(path);
	}
	playAnimation(makePosition(300, 300,3), gData.numbers, numberAnimation, makeRectangleFromTexture(gData.numbers[0]), timerFinished, NULL);
	
	gData.distancePosition = makePosition(170, 100, 10);
	gData.point = loadTexture("assets/sprites/POINT.pkg");
	gData.miles = loadTexture("assets/sprites/MILES.pkg");

	gData.distanceTexture = loadTexture("assets/sprites/DISTANCE_TRAVELLED.pkg");
	playAnimationLoop(makePosition(140, 30, 10), &gData.distanceTexture, createOneFrameAnimation(), makeRectangleFromTexture(gData.distanceTexture));

	gData.isTimerFinished = 0;
}


static void unloadRetryScreen() {

}


static void updateRetryScreen() {
	updateStage();
}

static void drawTravelledDistance() { // TODO: import playbill font properly (will never ever happen though, but hey)
	gData.distanceTravelled = fmin(gData.distanceTravelled, 99.99);
	
	int dist = (int)gData.distanceTravelled;

	int firstDig = (dist / 10) % 10;
	int secondDig = dist % 10;

	double afterDigs = gData.distanceTravelled - dist;

	int firstCommaDig = (int)(afterDigs * 10);
	int secondCommaDig = ((int)(afterDigs * 100)) % 10;
	
	Position p = gData.distancePosition;
	double dx = gData.numbers[0].mTextureSize.x - 35;

	if (firstDig > 0) {
		drawSprite(gData.numbers[9-firstDig], p, makeRectangleFromTexture(gData.numbers[0]));
		p.x += dx;
	}
	else {
		p.x += dx / 2;
	}

	drawSprite(gData.numbers[9 - secondDig], p, makeRectangleFromTexture(gData.numbers[0]));
	p.x += dx;

	drawSprite(gData.point, p, makeRectangleFromTexture(gData.point));
	p.x += dx;

	drawSprite(gData.numbers[9 - firstCommaDig], p, makeRectangleFromTexture(gData.numbers[0]));
	p.x += dx;

	drawSprite(gData.numbers[9 - secondCommaDig], p, makeRectangleFromTexture(gData.numbers[0]));
	p.x += dx;

	p.x += dx;

	drawSprite(gData.miles, p, makeRectangleFromTexture(gData.miles));

} 

static void drawRetryScreen() {
	drawTravelledDistance();
}

static Screen* getNextRetryScreenScreen() {
	if (hasPressedAbortFlank()) {
		return &TitleScreen;
	}

	else if (hasPressedStartFlank()) {
		return &GameScreen;
	}
	else if (gData.isTimerFinished) {
		return &TitleScreen;
	}

	return NULL;
}


Screen RetryScreen = {
	.mLoad = loadRetryScreen,
	.mUnload = unloadRetryScreen,
	.mUpdate = updateRetryScreen,
	.mDraw = drawRetryScreen,
	.mGetNextScreen = getNextRetryScreenScreen
};
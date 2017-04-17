#include "stage.h"

#include <stdio.h>

#include <tari/stagehandler.h>

static struct {
	int backPlane;
	int spritePlane;

	PhysicsObject backPhysics;
	PhysicsObject spritePhysics;

	int hasSavedPhysics;

	double distanceTravelled;

} gData;

void loadStage() {
	setStageHandlerNoDelayedLoading();

	gData.backPlane = addScrollingBackground(0.1, 1);
	setScrollingBackgroundMaxVelocity(gData.backPlane, 0.1);
	
	gData.spritePlane = addScrollingBackground(2, 2);
	setScrollingBackgroundMaxVelocity(gData.spritePlane, 20);

	addBackgroundElement(gData.backPlane, makePosition(0,0,0), "assets/sprites/BG_BACK.pkg", createOneFrameAnimation());
	addBackgroundElement(gData.backPlane, makePosition(639,0,0), "assets/sprites/BG_BACK.pkg", createOneFrameAnimation());

	addBackgroundElement(gData.spritePlane, makePosition(0, 0, 0), "assets/sprites/BG_FRONT.pkg", createOneFrameAnimation());
	addBackgroundElement(gData.spritePlane, makePosition(639, 0, 0), "assets/sprites/BG_FRONT.pkg", createOneFrameAnimation());

	if (gData.hasSavedPhysics) {
		setScrollingBackgroundPhysics(gData.backPlane, gData.backPhysics);
		setScrollingBackgroundPhysics(gData.spritePlane, gData.spritePhysics);
	}

	gData.distanceTravelled = 0;
}

static void savePhysics() {
	gData.backPhysics = *getScrollingBackgroundPhysics(gData.backPlane);
	gData.spritePhysics = *getScrollingBackgroundPhysics(gData.spritePlane);
	gData.hasSavedPhysics = 1;
}

static void updateSingleScrollingBackground(int id) {
	Position p = *getScrollingBackgroundPositionReference(id);

	if (p.x < 640) return;

	p.x -= 640;
	setScrollingBackgroundPosition(id, p);
}

static void updateDistance() {
	double horseSpeedMilesPerTick = 2 * 29.83 / (60 *  60 * 60);
	gData.distanceTravelled += horseSpeedMilesPerTick;
}

void updateStage() {
	updateSingleScrollingBackground(gData.spritePlane);
	updateSingleScrollingBackground(gData.backPlane);

	savePhysics();
	scrollBackgroundRight(1);
	
	updateDistance();
}

double getDistanceTravelledDuringLastScreen() {
	return gData.distanceTravelled;
}

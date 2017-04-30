#include "shadow.h"

#include <tari/animation.h>

static struct {
	TextureData shadowTexture;
} gData;

void loadShadows() {
	gData.shadowTexture = loadTexture("assets/sprites/SHADOW.pkg");
}

int addShadow(Position* referencePosition) {
	int id = playAnimationLoop(makePosition(0,32,-2), &gData.shadowTexture, createOneFrameAnimation(), makeRectangleFromTexture(gData.shadowTexture));
	setAnimationBasePositionReference(id, referencePosition);
	return id;
}

void removeShadow(int id) {
	removeHandledAnimation(id);
}
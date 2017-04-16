#include "kidnapper.h"

#include <tari/animation.h>
#include <tari/file.h>
#include <tari/physicshandler.h>

#include "shadow.h"

static struct {
	int physicsID;
	int animationID;
	int shadowID;

	TextureData kidnapperTextures[10];
	Animation kidnapperAnimation;

	Position* pos;

	int isActive;
} gData;

void loadKidnapper() {
	
	gData.kidnapperAnimation = createEmptyAnimation();
	gData.kidnapperAnimation.mFrameAmount = 6;
	gData.kidnapperAnimation.mDuration = 4;

	int i;
	for (i = 0; i < (int)gData.kidnapperAnimation.mFrameAmount; i++) {
		char path[1024];
		getPathWithNumberAffixedFromAssetPath(path, "assets/sprites/KIDNAPPER.pkg", i);
		gData.kidnapperTextures[i] = loadTexture(path);
	}

	gData.physicsID = addToPhysicsHandler(makePosition(450, 350, 5));
	gData.pos = &getPhysicsFromHandler(gData.physicsID)->mPosition;
	addAccelerationToHandledPhysics(gData.physicsID, makePosition(2, 0, 0));

	gData.animationID = playAnimationLoop(makePosition(0,0,0), gData.kidnapperTextures, gData.kidnapperAnimation, makeRectangleFromTexture(gData.kidnapperTextures[0]));
	setAnimationBasePositionReference(gData.animationID, gData.pos);
	
	gData.shadowID = addShadow(gData.pos);

	gData.isActive = 1;
}

static void removeKidnapper() {
	removeShadow(gData.shadowID);
	removeHandledAnimation(gData.animationID);
	removeFromPhysicsHandler(gData.physicsID);

	gData.isActive = 0;
}

void updateKidnapper() {
	if (!gData.isActive) return;

	if (gData.pos->x > 670) {
		removeKidnapper();
	}


}
#include "manualenemy.h"

#include <stdio.h>

#include <tari/texture.h>
#include <tari/input.h>
#include <tari/animation.h>
#include <tari/physicshandler.h>
#include <tari/math.h>

#include "enemies.h"

static struct {
	int isActive;

	TextureData pointer;
	TextureData text;

	Position* pos;
	double angle;
	double scale;

	int physicsID;
	int pointerID;
	Position pointerCenter;
	int textID;

	Duration shotCooldownNow;
	Duration shotCooldown;
	int isInCooldown;
} gData;

void loadManualEnemy() {
	gData.isActive = 0;
	gData.pointer = loadTexture("assets/sprites/POINTER.pkg");
	gData.pointerCenter = makePosition(0, gData.pointer.mTextureSize.y / 2, 0);
	gData.text = loadTexture("assets/sprites/MANUAL.pkg");

	gData.shotCooldownNow = 0;
	gData.shotCooldown = 30;
	gData.isInCooldown = 0;
}

int isManual() {
	return gData.isActive;
}

static void setStatusActive() {
	if (gData.isActive) return;

	gData.physicsID = addToPhysicsHandler(makePosition(300, 20, 3));
	setHandledPhysicsDragCoefficient(gData.physicsID, makePosition(0.2, 0.2, 0));
	setHandledPhysicsMaxVelocity(gData.physicsID, 4);
	gData.pos = getHandledPhysicsPositionReference(gData.physicsID);

	gData.pointerID = playOneFrameAnimationLoop(makePosition(0,0,0), &gData.pointer);
	setAnimationBasePositionReference(gData.pointerID, gData.pos);
	gData.scale = 0;
	setAnimationScale(gData.pointerID, makePosition(gData.scale, gData.scale, 1), gData.pointerCenter);

	gData.angle = -M_PI / 2;
	setAnimationRotationZ(gData.pointerID, gData.angle, gData.pointerCenter);

	gData.textID = playOneFrameAnimationLoop(makePosition(500, 428, 13), &gData.text);

	gData.isActive = 1;
}

static void setStatusInactive() {
	if (!gData.isActive) return;

	removeHandledAnimation(gData.pointerID);
	removeHandledAnimation(gData.textID);
	removeFromPhysicsHandler(gData.physicsID);

	gData.isActive = 0;
}

static void updateStatusInput() {
	if (hasPressedStartFlankSingle(1)) {
		if (gData.isActive) {
			setStatusInactive();
		}
		else {
			setStatusActive();
		}
	}
}

static void shoot() {
	double scale = randfrom(0.8, 1.2);
	Position p = makePosition(gData.pos->x-64, -scale*128, gData.pos->z+1);
	Velocity v = vecRotateZ(makePosition(gData.scale, 0, 0), -gData.angle);
	
	addEnemy(p, v, scale);
	gData.isInCooldown = 1;
}

static void updateShot() {
	int cannotShoot = gData.isInCooldown && !handleDurationAndCheckIfOver(&gData.shotCooldownNow, gData.shotCooldown);
	cannotShoot |= gData.scale == 0;
	if (cannotShoot) return;

	gData.isInCooldown = 0;
	gData.shotCooldownNow = 0;

	if (hasPressedRSingle(1)) {
		shoot();
	}
}

static void updatePointer() {
	double dx = getSingleLeftStickNormalizedX(1);
	double dy = getSingleLeftStickNormalizedY(1);
	Vector3D stick = makePosition(-dx, -dy, 0);
	gData.scale = fmin(vecLength(stick) * 2, 2.0);
	setAnimationScale(gData.pointerID, makePosition(gData.scale, gData.scale, 0), gData.pointerCenter);

	if (gData.scale < 0.01) return;

	gData.angle = getAngleFromDirection(stick);
	setAnimationRotationZ(gData.pointerID, gData.angle, gData.pointerCenter);

}

static void updateShootingInput() {
	if (!gData.isActive) return;

	updateShot();
	updatePointer();
}

static void updateMovementInput() {
	int cannotMove = !gData.isActive || gData.scale == 0;
	if (cannotMove) return;

	gData.pos->x = fmax(-0, fmin(640, gData.pos->x));

	double accel = 1;
	if (hasPressedXSingle(1)) {
		addAccelerationToHandledPhysics(gData.physicsID, makeAcceleration(-accel, 0, 0));
	}
	else if (hasPressedBSingle(1)) {
		addAccelerationToHandledPhysics(gData.physicsID, makeAcceleration(accel, 0, 0));
	}
}

static void updateManualEnemyInput() {
	updateStatusInput();
	updateShootingInput();
	updateMovementInput();
}

void updateManualEnemy() {
	updateManualEnemyInput();
}

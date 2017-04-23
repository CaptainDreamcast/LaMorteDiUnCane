#include "player.h"

#include <math.h>

#include <tari/animationtree.h>
#include <tari/input.h>
#include <tari/physicshandler.h>
#include <tari/file.h>
#include <tari/math.h>
#include <tari/collisionhandler.h>
#include <tari/system.h>
#include <tari/timer.h>
#include <tari/math.h>

#include "bullet.h"
#include "collision.h"
#include "gamelogic.h"
#include "shadow.h"

static struct {
	int animationID;
	int physicsID;
	int collisionID;
	int shadowID;

	int armAnimationID;

	Animation idleAnimation;
	TextureData idleTextures[10];

	Animation deathAnimation;
	TextureData deathTextures[10];

	TextureData armTexture;
	double armAngle;
	Position armRotationPoint;
	Position armPosition;

	int isInShotCooldown;
	Duration shotCooldownNow;
	Duration shotCooldown;

	Position* pos;

	CollisionData colData;

	double accel;

	int isDying;

} gData;

static void loadPlayerTextures() {

	int i;

	gData.idleAnimation = createEmptyAnimation();
	gData.idleAnimation.mFrameAmount = 6;
	gData.idleAnimation.mDuration = 3;

	for (i = 0; i < (int)gData.idleAnimation.mFrameAmount; i++) {
		char path[1024];
		getPathWithNumberAffixedFromAssetPath(path, "assets/sprites/RIDING.pkg", i);
		gData.idleTextures[i] = loadTexture(path);
	}

	gData.deathAnimation = createEmptyAnimation();
	gData.deathAnimation.mFrameAmount = 5;
	gData.deathAnimation.mDuration = 7;

	for (i = 0; i < (int)gData.deathAnimation.mFrameAmount; i++) {
		char path[1024];
		getPathWithNumberAffixedFromAssetPath(path, "assets/sprites/DEATH.pkg", i);
		gData.deathTextures[i] = loadTexture(path);
	}

	gData.armTexture = loadTexture("assets/sprites/ARM.pkg");
}

void playerHit(void* caller, void* collisionData);

void loadPlayer() {
	loadPlayerTextures();

	gData.physicsID = addToPhysicsHandler(makePosition(100,350,6));
	setHandledPhysicsDragCoefficient(gData.physicsID, makePosition(0.2, 0.2, 0));
	setHandledPhysicsMaxVelocity(gData.physicsID, 4);
	gData.accel = 1;
	gData.pos = &getPhysicsFromHandler(gData.physicsID)->mPosition;


	gData.animationID = playAnimationLoop(makePosition(0,0,0), gData.idleTextures, gData.idleAnimation, makeRectangleFromTexture(gData.idleTextures[0]));
	setAnimationBasePositionReference(gData.animationID, gData.pos);
	gData.armAnimationID = -1;
	gData.armAngle = 0;
	gData.armRotationPoint = makePosition(0, 7, 0);
	gData.armPosition = makePosition(70, 20, -1);

	gData.shadowID = addShadow(gData.pos);

	gData.isInShotCooldown = 0;
	gData.shotCooldownNow = 0;
	gData.shotCooldown = 20;

	gData.colData.listID = getPlayerCollisionListID();
	gData.colData.direction = &getPhysicsFromHandler(gData.physicsID)->mVelocity;
	CollisionRect rect = makeCollisionRect(makePosition(48, 32, 0), makePosition(48+40, 32+18, 1));
	Collider collider = makeColliderFromRect(rect);
	gData.collisionID = addColliderToCollisionHandler(getPlayerCollisionListID(), gData.pos, collider, playerHit, NULL, &gData.colData);	

	gData.isDying = 0;
}

static void playerDied(void* caller) {
	removeFromPhysicsHandler(gData.physicsID);
	setLevelLost();
}

static void setArmActive(double angle) {
	if (gData.armAnimationID == -1) {
		gData.armAnimationID = playAnimationLoop(gData.armPosition, &gData.armTexture, createOneFrameAnimation(), makeRectangleFromTexture(gData.armTexture));
		setAnimationBasePositionReference(gData.armAnimationID, gData.pos);
	}

	setAnimationRotationZ(gData.armAnimationID, angle, gData.armRotationPoint);
}

static void setArmInactive() {
	if (gData.armAnimationID == -1) return;
	removeHandledAnimation(gData.armAnimationID);
	gData.armAnimationID = -1;
}

void playerHit(void* caller, void* collisionData) {
	
	removeHandledAnimation(gData.animationID);
	removeFromCollisionHandler(getPlayerCollisionListID(), gData.collisionID);
	setArmInactive();

	int id = playAnimation(makePosition(0,0,0), gData.deathTextures, gData.deathAnimation, makeRectangleFromTexture(gData.deathTextures[0]), NULL, NULL);
	setAnimationBasePositionReference(id, gData.pos);
	addAccelerationToHandledPhysics(gData.physicsID, makePosition(-20, 0, 0));
	setHandledPhysicsDragCoefficient(gData.physicsID, makePosition(0, 0, 0));
	setHandledPhysicsMaxVelocity(gData.physicsID, INF);
	addTimerCB(120, playerDied, NULL);
	gData.isDying = 1;
}

static void handleArmDirection() {
	double dx = getLeftStickNormalizedX();
	double dy = getLeftStickNormalizedY();
	double l = fstsqrt(dx*dx + dy*dy);

	if (l < 0.5) {
		setArmInactive();
		return;
	}

	gData.armAngle = getAngleFromDirection(makePosition(-dx, -dy, 0)); // TODO: fix this with tests
	setArmActive(gData.armAngle);
}

static void handleShotCooldown() {
	if (!gData.isInShotCooldown) return;

	if (handleDurationAndCheckIfOver(&gData.shotCooldownNow, gData.shotCooldown)) {
		gData.isInShotCooldown = 0;
	}
}

static void handleShotFireAttempt() {
	if (gData.isInShotCooldown || gData.armAnimationID == -1) return;

	Position bulletPos = vecAdd(*gData.pos, vecRotateZ(makePosition(35, 0, 0), -gData.armAngle));
	bulletPos = vecAdd(vecAdd(bulletPos, gData.armRotationPoint), gData.armPosition);

	addBullet(bulletPos, gData.armAngle);

	gData.shotCooldownNow = 0;
	gData.isInShotCooldown = 1;
}

static void handleShots() {

	handleShotCooldown();
	
	if (hasPressedR()) {
		handleShotFireAttempt();
	}


}

static void handleMovement() {

	if (hasPressedX()) {
		addAccelerationToHandledPhysics(gData.physicsID, makePosition(-gData.accel, 0, 0));
	} else if (hasPressedB()) {
		addAccelerationToHandledPhysics(gData.physicsID, makePosition(gData.accel, 0, 0));
	}

}

static void constraintPlayerPosition() {
	gData.pos->x = fmax(-64, gData.pos->x);
	gData.pos->x = fmin(576, gData.pos->x);

}
	


void updatePlayer() {
	if (gData.isDying) return;

	constraintPlayerPosition();
	handleArmDirection();

	handleShots();

	handleMovement();
}

#include "bullet.h"

#include <stdlib.h>

#include <tari/datastructures.h>
#include <tari/geometry.h>
#include <tari/animation.h>
#include <tari/physicshandler.h>
#include <tari/memoryhandler.h>
#include <tari/collisionhandler.h>
#include <tari/log.h>
#include <tari/math.h>

#include "collision.h"

typedef struct {
	Position* pos;
	int physicsID;
	int animationID;
	int collisionID;

	CollisionData colData;
} Bullet;

static struct {
	List bullets;

	TextureData bulletTexture;

	TextureData bulletsLeftTexture;
	int bulletsLeftID;

	int bulletsLeft;
	int bulletsLeftDisplayIDs[6];
} gData;



void setupBulletHandling() {
	gData.bullets = new_list();
	gData.bulletTexture = loadTexture("assets/sprites/BULLET.pkg");

	gData.bulletsLeftTexture = loadTexture("assets/sprites/BULLETSLEFT.pkg");
	gData.bulletsLeftID = playOneFrameAnimationLoop(makePosition(20, 396, 13), &gData.bulletsLeftTexture);
	setAnimationScale(gData.bulletsLeftID, makePosition(0.75, 0.75, 1), makePosition(0, 32, 0));
	gData.bulletsLeft = 0;

	int i;
	for (i = 0; i < 6; i++) {
		double x = 20 + 0.75*32 * i;
		gData.bulletsLeftDisplayIDs[i] = playOneFrameAnimationLoop(makePosition(x, 428, 13), &gData.bulletTexture);
		setAnimationRotationZ(gData.bulletsLeftDisplayIDs[i], M_PI / 4, makePosition(16, 16, 0));
		reloadBullet();
	}
}

void reloadBullet() {
	if (gData.bulletsLeft == 6) return;
	
	int i = gData.bulletsLeft;
	setAnimationScale(gData.bulletsLeftDisplayIDs[i], makePosition(0.75, 0.75, 1), makePosition(16, 16, 0));

	gData.bulletsLeft++;
}

int getBulletAmount() {
	return gData.bulletsLeft;
}

static void removeBulletFromPistol() {
	if (!gData.bulletsLeft) {
		logWarning("Trying to remove bullet from pistol without bullets.");
		return;
	}

	int i = gData.bulletsLeft - 1;
	setAnimationScale(gData.bulletsLeftDisplayIDs[i], makePosition(0, 0, 0), makePosition(16, 16, 0));

	gData.bulletsLeft--;
}

static void removeBullet(Bullet* e) {
	removeHandledAnimation(e->animationID);
	removeFromPhysicsHandler(e->physicsID);
	removeFromCollisionHandler(getBulletCollisionListID(), e->collisionID);
}

static int deleteAllBullets(void* caller, void* data) {
	Bullet* e = data;
	removeBullet(e);

	return 1;
}

void shutdownBulletHandling() {
	list_remove_predicate(&gData.bullets, deleteAllBullets, NULL);
}

static int updateBullet(void* caller, void* data) {
	Bullet* e = data;

	Position p = *(e->pos);
	if (p.x < -200 || p.x > 700 || p.y < -200 || p.y > 700) {
		removeBullet(e);
		return 1;
	}

	return 0;
}

void updateBulletHandling() {
	list_remove_predicate(&gData.bullets, updateBullet, NULL);

}

static void bulletHit(void* caller, void* collisionData) {
	Bullet* e = caller;
	
	e->pos->x = -300;
}

void addBullet(Position pos, double angle) {
	Bullet* e = allocMemory(sizeof(Bullet));

	e->physicsID = addToPhysicsHandler(pos);
	e->pos = &getPhysicsFromHandler(e->physicsID)->mPosition;
	addAccelerationToHandledPhysics(e->physicsID, vecRotateZ(makePosition(10,0,0), -angle));
	e->animationID = playAnimationLoop(makePosition(-16,-16,0), &gData.bulletTexture, createOneFrameAnimation(), makeRectangleFromTexture(gData.bulletTexture));
	setAnimationBasePositionReference(e->animationID, e->pos);
	setAnimationRotationZ(e->animationID, angle, makePosition(16,16,0));
	
	e->colData.listID = getBulletCollisionListID();
	e->colData.direction = &getPhysicsFromHandler(e->physicsID)->mVelocity;

	e->collisionID = addColliderToCollisionHandler(getBulletCollisionListID(), e->pos, makeColliderFromCirc(makeCollisionCirc(makePosition(0, 0, 0), 16)), bulletHit, e, &e->colData);
	list_push_back_owned(&gData.bullets, e);

	removeBulletFromPistol();
}

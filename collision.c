#include "collision.h"

#include <tari/collisionhandler.h>

static struct {
	int enemies;
	int bullets;
	int players;
} gData;

void loadCollisions() {
	gData.enemies = addCollisionListToHandler();
	gData.bullets = addCollisionListToHandler();
	gData.players = addCollisionListToHandler();

	addCollisionHandlerCheck(gData.bullets, gData.enemies);
	addCollisionHandlerCheck(gData.players, gData.enemies);
	setCollisionHandlerOwningColliders();
}

int getEnemyCollisionListID() {
	return gData.enemies;
}

int getBulletCollisionListID() {
	return gData.bullets;
}

int getPlayerCollisionListID() {
	return gData.players;
}
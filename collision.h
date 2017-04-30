#pragma once

#include <tari/physics.h>

typedef struct {
	int listID;
	Velocity* direction;
} CollisionData;

void loadCollisions();
int getEnemyCollisionListID();
int getBulletCollisionListID();
int getPlayerCollisionListID();
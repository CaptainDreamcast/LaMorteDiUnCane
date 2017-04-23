#include "enemies.h"

#include <math.h>

#include <tari/texture.h>
#include <tari/animation.h>
#include <tari/file.h>
#include <tari/datastructures.h>
#include <tari/math.h>
#include <tari/memoryhandler.h>
#include <tari/physicshandler.h>
#include <tari/collisionhandler.h>


#include "collision.h"
#include "manualenemy.h"

typedef struct {
	int physicsID;
} DeadEnemy;

typedef struct {
	int animationID;
	int physicsID;
	int collisionID;

	Position* pos;
	Velocity* vel;

	double scale;
	double rotation;

	CollisionData colData;

} Enemy;

static struct {
	TextureData enemyTexture[10];
	Animation enemyAnimation;

	TextureData enemyDeathTexture[10];
	Animation enemyDeathAnimation;

	List enemies;

	double z;

	double enemyCreationProb;
} gData;

static void loadEnemyTextures() {

	int i;

	gData.enemyAnimation = createEmptyAnimation();
	gData.enemyAnimation.mFrameAmount = 1;

	for (i = 0; i < (int)gData.enemyAnimation.mFrameAmount; i++) {
		char path[1024];
		getPathWithNumberAffixedFromAssetPath(path, "assets/sprites/ENEMY.pkg", i);
		gData.enemyTexture[i] = loadTexture(path);
	}

	gData.enemyDeathAnimation = createEmptyAnimation();
	gData.enemyDeathAnimation.mFrameAmount = 5;
	gData.enemyDeathAnimation.mDuration = 7;


	for (i = 0; i < (int)gData.enemyDeathAnimation.mFrameAmount; i++) {
		char path[1024];
		getPathWithNumberAffixedFromAssetPath(path, "assets/sprites/ENEMY_DEATH.pkg", i);
		gData.enemyDeathTexture[i] = loadTexture(path);
	}
}

void loadEnemies() {
	loadEnemyTextures();

	gData.z = 4;
	gData.enemyCreationProb = 2;

	gData.enemies = new_list();
}

static void removeEnemy(Enemy* e) {
	removeFromPhysicsHandler(e->physicsID);
	removeHandledAnimation(e->animationID);
	removeFromCollisionHandler(getEnemyCollisionListID(), e->collisionID);
}

void enemyHitCB(void* caller, void* collisionData);

void addEnemy(Position pos, Velocity vel, double scale) {
	Enemy* e = allocMemory(sizeof(Enemy));

	e->physicsID = addToPhysicsHandler(pos);
	PhysicsObject* physics = getPhysicsFromHandler(e->physicsID);
	e->pos = &physics->mPosition;
	e->vel = &physics->mVelocity;
	addAccelerationToHandledPhysics(e->physicsID, vel);
	setHandledPhysicsGravity(e->physicsID, makePosition(0, 0.07, 0));

	e->animationID = playAnimationLoop(makePosition(0, 0, 0), gData.enemyTexture, gData.enemyAnimation, makeRectangleFromTexture(gData.enemyTexture[0]));
	setAnimationBasePositionReference(e->animationID, e->pos);

	e->scale = scale;
	e->rotation = randfrom(0, M_PI);

	Position center = makePosition(gData.enemyTexture[0].mTextureSize.x / 2, gData.enemyTexture[0].mTextureSize.y / 2, 0);
	setAnimationScale(e->animationID, makePosition(e->scale, e->scale, 1), center);
	setAnimationRotationZ(e->animationID, e->rotation, center);

	e->colData.listID = getEnemyCollisionListID();
	e->colData.direction = e->vel;
	e->collisionID = addColliderToCollisionHandler(getEnemyCollisionListID(), e->pos, makeColliderFromCirc(makeCollisionCirc(center, center.x * e->scale)), enemyHitCB, e, &e->colData);

	list_push_back_owned(&gData.enemies, e);
}

static void splitupEnemy(Enemy* e, CollisionData* c) {
	Position baseP = *e->pos;
	Velocity baseEnemyShot = normalizeVelocity(*e->vel);
	Velocity basePlayerShot = normalizeVelocity(*c->direction);
	Velocity baseV = normalizeVelocity(vecAdd(vecScale(baseEnemyShot, 1), vecScale(basePlayerShot, 0)));

	double speed = vecLength(*e->vel)*0.5;
	baseV = vecScale(baseV, speed);

	Velocity v1 = vecRotateZ(baseV, degreesToRadians(90));
	Velocity v2 = vecRotateZ(baseV, degreesToRadians(-90));

	if (e->scale < 0.4) return;

	addEnemy(baseP, v1, e->scale / 2);
	addEnemy(baseP, v2, e->scale / 2);
}

void enemyHitCB(void* caller, void* collisionData) {
	Enemy* e = caller;
	CollisionData* c = collisionData;

	if (c->listID == getPlayerCollisionListID()) return;

	splitupEnemy(e, c);
	e->pos->x = -1000;
	e->pos->y = 1000;
}

static void createNewEnemy() {
	
	double x = randfrom(-10, 700);
	double y = -150;

	double dx = randfrom(-3, 3);
	double dy = randfrom(0.1, 5);

	Position pos = makePosition(x, y, gData.z);
	Velocity vel = makePosition(dx, dy, 0);

	double scale = randfrom(0.5, 1);

	addEnemy(pos, vel, scale);
}

static void handleEnemyCreation() {
	if (isManual()) return;

	double prob = randfrom(0,100);

	if (prob > gData.enemyCreationProb) {
		gData.enemyCreationProb += 0.005;
		return;
	}
	createNewEnemy();
}

static void removeDeadEnemy(void* caller) {
	DeadEnemy* e = caller;
	removeFromPhysicsHandler(e->physicsID);
	freeMemory(e);
}

static void playEnemyDeathAnimation(Position p, double scale) {
	DeadEnemy* e = allocMemory(sizeof(DeadEnemy));

	p.y -= gData.enemyDeathTexture[0].mTextureSize.y - gData.enemyTexture[0].mTextureSize.y;
	
	e->physicsID = addToPhysicsHandler(p);
	addAccelerationToHandledPhysics(e->physicsID, makePosition(-20, 0, 0));
	Position* pos = &getPhysicsFromHandler(e->physicsID)->mPosition;

	Position c = makePosition(gData.enemyDeathTexture[0].mTextureSize.x / 2, gData.enemyTexture[0].mTextureSize.y + (gData.enemyTexture[0].mTextureSize.y / 2), 0);
	int id = playAnimation(makePosition(0,0,0), gData.enemyDeathTexture, gData.enemyDeathAnimation, makeRectangleFromTexture(gData.enemyDeathTexture[0]), removeDeadEnemy, e);
	setAnimationScale(id, makePosition(scale, scale, 1), c);
	setAnimationBasePositionReference(id, pos);
}

static int handleSingleEnemyUpdate(void* caller, void* data) {
	Enemy* e = data;
	
	e->rotation += (1-e->scale)*0.2;
	Position c = makePosition(gData.enemyTexture[0].mTextureSize.x / 2, gData.enemyTexture[0].mTextureSize.y / 2, 0);
	setAnimationRotationZ(e->animationID, e->rotation, c);

	Position p = *e->pos;
	double scale = e->scale;
	double dy = gData.enemyTexture[0].mTextureSize.y;
	double uy = p.y + dy / 2 + scale*(dy / 2);

	if (p.x < -200 || p.y < -300 || p.x > 700 || uy > 428) {
		removeEnemy(e);
		if (uy > 428) {
			playEnemyDeathAnimation(p, scale);
		}
		return 1;
	}

	return 0;
}

static void handleEnemyUpdate() {
	list_remove_predicate(&gData.enemies, handleSingleEnemyUpdate, NULL);
}

void updateEnemies() {
	handleEnemyCreation();

	handleEnemyUpdate();
}

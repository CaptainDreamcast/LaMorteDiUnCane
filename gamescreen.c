#include "gamescreen.h"

#include <stdlib.h>

#include <tari/input.h>
#include <tari/wrapper.h>
#include <tari/animationtree.h>
#include <tari/stagehandler.h>
#include <tari/collisionhandler.h>

#include "player.h"
#include "bullet.h"
#include "stage.h"
#include "enemies.h"
#include "ui.h"
#include "collision.h"
#include "titlescreen.h"
#include "gamelogic.h"
#include "retryscreen.h"
#include "shadow.h"
#include "kidnapper.h"
#include "manualenemy.h"

static void loadGameScreen() {

	// activateCollisionHandlerDebugMode();
	
	loadGameLogic();
	loadCollisions();
	loadShadows();
	loadStage();
	setupBulletHandling();
	loadPlayer();
	loadKidnapper();
	loadEnemies();
	loadManualEnemy();
	loadUI();
	
}

static void unloadGameScreen() {
	shutdownBulletHandling();
}

static void updateGameScreen() {
	updateBulletHandling();
	updatePlayer();
	updateKidnapper();
	updateEnemies();
	updateManualEnemy();
	updateStage();
	updateUI();
}

static void drawGameScreen() {
	
}

static Screen* getNextGameScreenScreen() {

	if (hasPressedAbortFlank()) {
		return &TitleScreen;
	}
	else if (isLevelLost()) {
		return &RetryScreen;
	}

	return NULL;
}



Screen GameScreen = {
	.mLoad = loadGameScreen,
	.mUnload = unloadGameScreen,
	.mUpdate = updateGameScreen,
	.mDraw = drawGameScreen,
	.mGetNextScreen = getNextGameScreenScreen
};
#include <tari/framerateselectscreen.h>
#include <tari/pvr.h>
#include <tari/physics.h>
#include <tari/file.h>
#include <tari/drawing.h>
#include <tari/log.h>
#include <tari/wrapper.h>
#include <tari/system.h>
#include <tari/stagehandler.h>

#include "gamescreen.h"
#include "titlescreen.h"
#include "retryscreen.h"
#include "logoscreen.h"

#ifdef DREAMCAST
KOS_INIT_FLAGS(INIT_DEFAULT);

extern uint8 romdisk[];
KOS_INIT_ROMDISK(romdisk);
#endif


void exitGame() {
	shutdownTariWrapper();

#define DEVELOP

#ifdef DEVELOP
	abortSystem();
#else
	returnToMenu();
#endif
}

void setMainFileSystem() {
#ifdef DEVELOP
	setFileSystem("/pc");
#else
	setFileSystem("/rd");
#endif
}

int main(int argc, char** argv) {

	setGameName("LA MORTE DI UN CANE");
	setScreenSize(640, 480);

	initTariWrapperWithDefaultFlags();

	logg("Check framerate");
	FramerateSelectReturnType framerateReturnType = selectFramerate();
	if (framerateReturnType == FRAMERATE_SCREEN_RETURN_ABORT) {
		exitGame();
	}

	setMainFileSystem();

	startScreenHandling(&LogoScreen);

	exitGame();


	return (1);
}


// TODO: find out why main doesn't work
int wmain(int argc, char** argv) {
	return main(argc, argv);
}



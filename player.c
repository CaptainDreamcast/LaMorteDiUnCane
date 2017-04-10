#include "player.h"

#include <tari/animationtree.h>
#include <tari/input.h>

static struct {
	int animationID;


} gData;

void loadPlayer() {
	gData.animationID = playAnimationTreeLoop(makePosition(100,100,4), loadAnimationTree("assets/player/PLAYERTREE.txt"), "IDLE");
}


void updatePlayer() {
	if (hasPressedAFlank()) {
		setHandledAnimationTreeAnimation(gData.animationID, "IDLE");
	}

	if (hasPressedBFlank()) {
		setHandledAnimationTreeAnimation(gData.animationID, "WHOA");
	}

}

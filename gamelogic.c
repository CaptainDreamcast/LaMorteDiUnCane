#include "gamelogic.h"

static struct {
	int isLevelLostFlag;

} gData;

void loadGameLogic() {
	gData.isLevelLostFlag = 0;
}

void setLevelLost() {
	gData.isLevelLostFlag = 1;
}

int isLevelLost() {
	return gData.isLevelLostFlag;
}
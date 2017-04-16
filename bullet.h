#pragma once

#include <tari/geometry.h>

void setupBulletHandling();
void shutdownBulletHandling();
void updateBulletHandling();

void addBullet(Position pos, double angle);
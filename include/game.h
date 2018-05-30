#pragma once

#include "alien.h"
#include "bullet.h"
#include "player.h"

struct Game {
    size_t width, height;
    size_t num_aliens;
    size_t num_bullets;
    Alien* aliens;
    Player player;
    Bullet bullets[GAME_MAX_BULLETS];
};

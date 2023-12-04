#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "object.h"

enum PlayerButtonFlags {
    PlayerButtonDuck = 0x01,
    PlayerButtonJump = 0x02,
};

struct PlayerCmd {
    vector_t wishdir;
    float mouseX;
    float mouseY;
    enum PlayerButtonFlags buttons;
    float frametime;
};

enum PlayerFlags {
    PlayerGrounded = 0x01,
    PlayerDucked = 0x02,
};

struct Player {
    struct Object object;

    enum PlayerFlags flags;
};

void player_init(struct Player* player, struct World* world);

#endif

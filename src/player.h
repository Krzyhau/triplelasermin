#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "math/transform.h"
#include "math/quaternion.h"


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
    struct World* world;

    transform_t transform;
    vector_t velocity;
    enum PlayerFlags flags;
};

void player_init(struct Player* player, struct World* world);
void player_update(struct Player* player, struct WindowHandler* window);

#endif

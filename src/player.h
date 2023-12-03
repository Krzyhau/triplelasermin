#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "math/transform.h"
#include "math/quaternion.h"

enum PlayerButtonFlags {
    PlayerButtonDuck = 0x01,
    PlayerButtonJump = 0x02
};

struct PlayerCmd {
    vector_t wishDir;
    enum PlayerButtonFlags buttons;
};

struct Player {
    struct World* world;

    transform_t transform;
    vector_t velocity;
};

void player_init(struct Player* player, struct World* world);
void player_update(struct Player* player, struct WindowHandler* window);

#endif

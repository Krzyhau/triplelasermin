#include "player.h"

#include <math.h>
#include "core/window.h"
#include "world.h"
#include "core/input.h"

void player_init(struct Player* player, struct World* world)
{
    player->world = world;
    quaternion_identity(&player->transform.rotation);
}

void player_create_input(struct Player* player, struct WindowHandler* window, struct PlayerCmd* outCmd) {
    // mouse
    const float sensitivity = 0.2f;

    outCmd->mouseX = window->input->deltaMouseX * sensitivity;
    outCmd->mouseY = window->input->deltaMouseY * sensitivity;

    // movement
    vector_t mov = { 0.0f, 0.0f, 0.0f, 0.0f };
    if (input_key_held(window->input, 'W')) {
        mov.z += 1.0f;
    }
    if (input_key_held(window->input, 'S')) {
        mov.z -= 1.0f;
    }
    if (input_key_held(window->input, 'A')) {
        mov.x -= 1.0f;
    }
    if (input_key_held(window->input, 'D')) {
        mov.x += 1.0f;
    }

    if (vector_len(mov) > 0) {
        vector_norm(mov, &mov);
    }
    outCmd->wishdir = mov;

    // buttons
    outCmd->buttons = 0;
    if (input_key_pressing(window->input, VK_SPACE)) {
        outCmd->buttons |= PlayerButtonJump;
    }
    if (input_key_pressing(window->input, VK_LCONTROL)) {
        outCmd->buttons |= PlayerButtonDuck;
    }
}

void player_apply_angles(struct Player* player, struct PlayerCmd* cmd) {
    quaternion_t pitchRot, yawRot;
    quaternion_axis_angle(-cmd->mouseY, (vector_t) { 1.0f, 0.0f, 0.0f }, & pitchRot);
    quaternion_axis_angle(-cmd->mouseX, (vector_t) { 0.0f, 1.0f, 0.0f }, & yawRot);

    quaternion_multiply(player->transform.rotation, yawRot, &player->transform.rotation);
    quaternion_multiply(pitchRot, player->transform.rotation, &player->transform.rotation);
}

void player_apply_friction(struct Player* player, struct PlayerCmd* cmd) {
    const float friction = 5;
    const float stopSpeed = 0.4f;

    if ((player->flags & PlayerGrounded)) {
        float speed2d = vector_len(player->velocity);

        float frictionScalar = 1.0f - cmd->frametime * friction;
        if (speed2d < stopSpeed && speed2d > 0.0f) {
            frictionScalar = fmaxf(speed2d - cmd->frametime * stopSpeed * friction, 0.0f) / speed2d;
        }
        vector_scale(player->velocity, frictionScalar, &player->velocity);
    }
}

void player_process_movement(struct Player* player, struct PlayerCmd* cmd) {
    const float maxGroundSpeed = 1.0f;
    const float maxAirSpeed = 0.4f;
    const float groundAccelForce = 10.0f;
    const float airAccelForce = 5.0f;

    player_apply_friction(player, cmd);

    // calculating wish direction
    vector_t forwardVec = { 0 };
    vector_t rightVec = { 0 };
    transform_right(player->transform, &rightVec);
    vector_cross(rightVec, (vector_t) { 0.0f, 1.0f, 0.0f }, &forwardVec);
    vector_scale(forwardVec, cmd->wishdir.z, &forwardVec);
    vector_scale(rightVec, cmd->wishdir.x, &rightVec);
    vector_t wishdir = { 0 };
    vector_add(forwardVec, rightVec, &wishdir);

    if (vector_len(wishdir) > 0) {
        // calculating acceleration force
        float duckMultiplier = ((player->flags & PlayerGrounded) && (player->flags & PlayerDucked)) ? (1.0f / 3.0f) : 1.0f;
        float maxSpeed = fminf(maxGroundSpeed, vector_len(wishdir) * maxGroundSpeed) * duckMultiplier;
        float maxAiredSpeed = (player->flags & PlayerGrounded) ? maxSpeed : fminf(maxSpeed, maxAirSpeed);

        float accelForce = (player->flags & PlayerGrounded) ? groundAccelForce : airAccelForce;
        float maxAccel = cmd->frametime * maxSpeed * accelForce;

        vector_norm(wishdir, &wishdir);
        // limiting acceleration
        float accelDiff = maxAiredSpeed - vector_dot(player->velocity, wishdir);

        float accel = fminf(fmaxf(accelDiff, 0.0f), maxAccel);

        vector_t accelVec;
        vector_scale(wishdir, accel, &accelVec);
        vector_add(accelVec, player->velocity, &player->velocity);
    }
}

void player_try_jump(struct Player* player, struct PlayerCmd* cmd) {
    if ((player->flags & PlayerGrounded) == 0) return;
    if ((cmd->buttons & PlayerButtonJump) == 0) return;

    player->velocity.y = 2.0f;
    player->flags &= ~PlayerGrounded;
}

void player_apply_gravity(struct Player* player, struct PlayerCmd* cmd) {
    if ((player->flags & PlayerGrounded)) {
        player->velocity.y = 0.0f;
    }
    else {
        player->velocity.y -= player->world->gravity * cmd->frametime;
    }
}

void player_update(struct Player* player, struct WindowHandler* window)
{
    
    struct PlayerCmd cmd;
    cmd.frametime = window->deltaTime;
    player_create_input(player, window, &cmd);
    player_try_jump(player, &cmd);
    player_apply_gravity(player, &cmd);
    player_apply_angles(player, &cmd);
    player_process_movement(player, &cmd);

    vector_t moveStep;
    vector_scale(player->velocity, window->deltaTime, &moveStep);
    vector_add(player->transform.position, moveStep, &player->transform.position);

    if (player->transform.position.y < 0.0f) {
        player->transform.position.y = 0;
        player->flags |= PlayerGrounded;
    }
}

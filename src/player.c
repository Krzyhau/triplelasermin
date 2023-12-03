#include "player.h"

#include "core/window.h"

void player_init(struct Player* player, struct World* world)
{
    player->world = world;
    quaternion_identity(&player->transform.rotation);
}

void player_update(struct Player* player, struct WindowHandler* window)
{
    // rotation
    float sensitivity = 0.2f;

    float pitchDelta = -window->input->deltaMouseY * sensitivity;
    float yawDelta = -window->input->deltaMouseX * sensitivity;

    quaternion_t pitchRot, yawRot;
    quaternion_axis_angle(pitchDelta, (vector_t) { 1.0f, 0.0f, 0.0f }, & pitchRot);
    quaternion_axis_angle(yawDelta, (vector_t) { 0.0f, 1.0f, 0.0f }, & yawRot);

    quaternion_multiply(player->transform.rotation, yawRot, &player->transform.rotation);
    quaternion_multiply(pitchRot, player->transform.rotation, &player->transform.rotation);

    // movement
    vector_t forwardVec;
    vector_t rightVec;
    transform_forward(player->transform, &forwardVec);
    transform_right(player->transform, &rightVec);

    vector_t mov = { 0.0f, 0.0f, 0.0f };

    if (input_key_held(window->input, 'W')) {
        vector_add(mov, forwardVec, &mov);
    }
    if (input_key_held(window->input, 'S')) {
        vector_sub(mov, forwardVec, &mov);
    }
    if (input_key_held(window->input, 'A')) {
        vector_sub(mov, rightVec, &mov);
    }
    if (input_key_held(window->input, 'D')) {
        vector_add(mov, rightVec, &mov);
    }
    if (input_key_held(window->input, 'Q')) {
        vector_add(mov, (vector_t) { 0.0f, 1.0f, 0.0f }, & mov);
    }
    if (input_key_held(window->input, 'E')) {
        vector_add(mov, (vector_t) { 0.0f, -1.0f, 0.0f }, & mov);
    }

    vector_scale(mov, 2.0f * window->deltaTime, &mov);

    vector_add(player->transform.position, mov, &player->transform.position);
}

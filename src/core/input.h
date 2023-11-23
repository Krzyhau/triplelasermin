#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdint.h>

#define INPUT_KEY_COUNT 255

enum InputKeyState {
    KeyStateReleased,
    KeyStatePressing,
    KeyStateTyping,
    KeyStateHeld,
    KeyStateReleasing,
};

enum InputKeyStateRequest {
    KeyStateRequestNone,
    KeyStateRequestPress,
    KeyStateRequestRelease,
};

struct InputState {
    enum InputKeyStateRequest* keyboardRequests;
    enum InputKeyState* keyboard;
};

void input_state_init(struct InputState* input);
void input_register_key_press(struct InputState* input, uint8_t keyCode);
void input_register_key_release(struct InputState* input, uint8_t keyCode);
void input_state_update(struct InputState* input);

enum InputKeyState input_key_state(struct InputState* input, uint8_t keyCode);
int input_key_pressing(struct InputState* input, uint8_t keyCode);
int input_key_typing(struct InputState* input, uint8_t keyCode);
int input_key_held(struct InputState* input, uint8_t keyCode);
int input_key_releasing(struct InputState* input, uint8_t keyCode);

#endif
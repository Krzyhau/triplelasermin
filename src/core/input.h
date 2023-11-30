#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdint.h>

#define INPUT_KEY_COUNT 256

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

enum InputMouseState {
    InputMouseFree,
    InputMouseLocked
};

struct InputState {
    enum InputKeyStateRequest keyboardRequests[INPUT_KEY_COUNT];
    enum InputKeyState keyboard[INPUT_KEY_COUNT];
    float mouseX;
    float mouseY;
    float realTimeMouseX;
    float realTimeMouseY;
    float deltaMouseX;
    float deltaMouseY;
    enum InputMouseState mouseLocked;
};

void input_state_init(struct InputState* input);
void input_register_key_press(struct InputState* input, uint8_t keyCode);
void input_register_key_release(struct InputState* input, uint8_t keyCode);
void input_register_mouse_movement(struct InputState* input, int mouseX, int mouseY);
void input_state_update(struct InputState* input);

enum InputKeyState input_key_state(struct InputState* input, uint8_t keyCode);
int input_key_pressing(struct InputState* input, uint8_t keyCode);
int input_key_typing(struct InputState* input, uint8_t keyCode);
int input_key_held(struct InputState* input, uint8_t keyCode);
int input_key_releasing(struct InputState* input, uint8_t keyCode);

#endif
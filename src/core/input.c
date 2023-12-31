#include "input.h"

#include <stdlib.h>

void input_state_init(struct InputState* input) {
    for (int i = 0; i < INPUT_KEY_COUNT; i++) {
        input->keyboard[i] = KeyStateReleased;
        input->keyboardRequests[i] = KeyStateRequestNone;
    }
}

void input_register_key_press(struct InputState* input, uint8_t keyCode)
{
    input->keyboardRequests[keyCode] = KeyStateRequestPress;
}

void input_register_key_release(struct InputState* input, uint8_t keyCode)
{
    input->keyboardRequests[keyCode] = KeyStateRequestRelease;
}

void input_register_mouse_movement(struct InputState* input, int mouseX, int mouseY)
{
    input->realTimeMouseX = mouseX;
    input->realTimeMouseY = mouseY;
}

void input_state_update(struct InputState* input)
{
    for (int i = 0; i < INPUT_KEY_COUNT; i++) {
        switch (input->keyboard[i]) {
            case KeyStatePressing: 
                input->keyboard[i] = KeyStateHeld; 
                break;
            case KeyStateTyping: 
                input->keyboard[i] = KeyStateHeld; 
                break;
            case KeyStateReleasing: 
                input->keyboard[i] = KeyStateReleased; 
                break;
        }

        switch (input->keyboardRequests[i]) {
            case KeyStateRequestPress: 
                input->keyboard[i] = (input->keyboard[i] & KeyStateHeld) 
                    ? KeyStateTyping : KeyStatePressing; 
                break;
            case KeyStateRequestRelease: 
                input->keyboard[i] = KeyStateReleasing; 
                break;
        }
        input->keyboardRequests[i] = KeyStateRequestNone;
    }

    input->deltaMouseX = input->realTimeMouseX - input->mouseX;
    input->deltaMouseY = input->realTimeMouseY - input->mouseY;
    input->mouseX = input->realTimeMouseX;
    input->mouseY = input->realTimeMouseY;
}


enum InputKeyState input_key_state(struct InputState* input, uint8_t keyCode)
{
    return input->keyboard[keyCode];
}

int input_key_pressing(struct InputState* input, uint8_t keyCode)
{
    enum InputKeyState state = input_key_state(input, keyCode);
    return
        state == KeyStatePressing;
}

int input_key_typing(struct InputState* input, uint8_t keyCode)
{
    enum InputKeyState state = input_key_state(input, keyCode);
    return
        state == KeyStatePressing ||
        state == KeyStateTyping;
}

int input_key_held(struct InputState* input, uint8_t keyCode)
{
    enum InputKeyState state = input_key_state(input, keyCode);
    return
        state != KeyStateReleased;
}

int input_key_releasing(struct InputState* input, uint8_t keyCode)
{
    enum InputKeyState state = input_key_state(input, keyCode);
    return
        state == KeyStateReleasing;
}

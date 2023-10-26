#include "Input.h"

Input::Input() {
    strobe = false;
    button_index = 0;
    button_state = 0;
}

void Input::set_button_state(uint8_t state) {
    strobe = state & 1;
    if(strobe){
        button_index = 0;
    }
}

uint8_t Input::read_button_state() {
    if(button_index > 7){
        return 1;
    }

    auto response = (button_state & (1 << button_index)) >> button_index;
    if(!strobe && button_index < 8){
        button_index++;
    }
    return response;
}

uint8_t Input::set_button_pressed_state(uint8_t button, bool pressed) {
    return button_state = pressed ? button_state | button : button_state & ~button;
}

void Input::reset() {
    strobe = false;
    button_index = 0;
    button_state = 0;
}
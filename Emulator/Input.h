#ifndef NES_EMULATOR_INPUT_H
#define NES_EMULATOR_INPUT_H

#include <cstdint>

#define RIGHT 0b10000000
#define LEFT 0b01000000
#define DOWN 0b00100000
#define UP 0b00010000
#define START 0b00001000
#define SELECT 0b00000100
#define BUTTON_B 0b00000010
#define BUTTON_A 0b00000001

class Input {
public:
    Input();
    ~Input() = default;

    void set_button_state(uint8_t state);
    uint8_t read_button_state();
    uint8_t set_button_pressed_state(uint8_t button, bool pressed);
    void reset();
private:
    bool strobe;
    uint8_t button_index;
    uint8_t button_state;

};


#endif //NES_EMULATOR_INPUT_H

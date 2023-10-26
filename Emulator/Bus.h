#ifndef NES_EMULATOR_BUS_H
#define NES_EMULATOR_BUS_H


#include <cstdint>
#include <array>
#include <iostream>
#include "ROM.h"
#include <functional>
#include <SDL2/SDL_video.h>
#include "Input.h"
#include "PPU.h"
#include "Interrupt_type.h"

class Bus {
    public:
        Bus();
        Bus(Mapper *mapper, SDL_Window *window);
        ~Bus() = default;
        uint8_t read_memory(uint16_t address);
        uint8_t read_prg_rom(uint16_t address);
        PPU get_ppu();
        void write_memory(uint16_t address, uint8_t data);
        int tick(uint8_t c);
        std::optional<uint8_t> poll_nmi_status();
        void set_gameloop_callback(std::function<void(Bus *)> callback);
        Input* get_joypad1();
        std::optional<Interrupt_type> Interrupt;
    std::vector<uint8_t> get_pixels();
    void render_frame();
    void reset();


private:
        std::array<uint8_t, 0x0800> vram;
        PPU ppu;
        int cycles;
        Mapper *mapper;

        Input joypad1;
        std::vector<uint8_t> pixels;
        SDL_Window *window;
};


#endif //NES_EMULATOR_BUS_H

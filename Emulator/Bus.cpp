#include "Bus.h"
#include "CPU.h"

#include <utility>
#include <fstream>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>

#define RAM_START 0x0000
#define MIRRORS_END 0x1FFF
#define PPU_START 0x2000
#define PPU_MIRRORS_END 0x3FFF
#define ROM_START 0x8000
#define ROM_END 0xFFFF


Bus::Bus() {
    vram.fill(0);
    cycles = 0;
}

Bus::Bus(Mapper *mapper, SDL_Window *window) {
    vram.fill(0);

    ppu = PPU(mapper);
    cycles = 0;

    joypad1 = Input();
    Interrupt = std::nullopt;
    this->mapper = mapper;
    this->window = window;
}





Input* Bus::get_joypad1() {
    return &joypad1;
}


uint8_t Bus::read_memory(uint16_t address) {

    switch (address) {
        case 0x2002:
            return ppu.read(address);
        case 0x2004:
            return ppu.read(address);
        case 0x2007:
            return ppu.read(address);
        case 0x4016:
            return joypad1.read_button_state();
        case 0x4017:
            return 0;
        default:
            break;
    }
    if(address >= 0x2008 && address <= 0x3FFF){
        ppu.read(address);
    }

    if(address >= 0x6000 && address <= 0x7FFF){
        return mapper->read(address);
    }

    if (address >= ROM_START && address <= ROM_END) {
        return mapper->read(address);
    }

    if (address >= RAM_START && address <= MIRRORS_END) {
        return vram[address & 0x07FF];
    }

    if (address >= PPU_START && address <= PPU_MIRRORS_END) {
        return read_memory(address & 0x2007);
    }

    // Invalid address
    std::cout << "Invalid address: " << address << std::endl;
    return 0;
}

void Bus::write_memory(uint16_t address, uint8_t data) {
   /* std::cout << "Write address: " << address << std::endl;*/
    switch(address) {
        case 0x0000 ... 0x1FFF: // RAM and mirrors
            vram[address & 0x07FF] = data;
            break;
        case 0x2000: // PPU control register 1
            ppu.write(0x2000, data);
            break;
        case 0x2001: // PPU control register 2
            ppu.write(0x2001, data);
            break;
        case 0x2002: // PPU status register (read-only)
            throw std::runtime_error("Read only address: " + std::to_string(address));
            break;
        case 0x2003: // PPU OAM address register
            ppu.write(0x2003, data);
            break;
        case 0x2004: // PPU OAM data register
            ppu.write(0x2004, data);
            break;
        case 0x2005: // PPU scroll register
            ppu.write(0x2005, data);
            break;
        case 0x2006: // PPU address register
            ppu.write(0x2006, data);
            break;
        case 0x2007: // PPU data register
            ppu.write(0x2007, data);
            break;
        case 0x2008 ... 0x3FFF: // PPU mirrors
            write_memory(address & 0x2007, data);
            break;
        case 0x4000 ... 0x4013: case 0x4015:
            // APU
            break;
        case 0x4014: // PPU OAM DMA register
        {
/*            std::array<uint8_t, 256> oam_data{};
            uint16_t high = data << 8;
            for(int i = 0; i < 256; i++){
                oam_data[i] = read_memory(high + i);
            }
            ppu.set_oam(oam_data);
            */

            uint16_t base = data << 8;
            for(int i = 0; i < 256; i++){
                ppu.write(0x2004, read_memory(base | i));
            }
        }
            break;
        case 0x4016: // Controller 1
            joypad1.set_button_state(data);
            break;
        case 0x4017: // Controller 2
            // Do nothing
            break;
        case 0x6000 ... 0x7FFF:
            mapper->write(address, data);
            break;
        case 0x8000 ... 0xFFFF: // ROM
            mapper->write(address, data);
            break;
        default:
            std::cout << "Invalid address: " << address << std::endl;
            break;
    }
}


int Bus::tick(uint8_t c){
   cycles += c;
   auto ppu_cycles = c * 3;

   auto frame_finished = false;
   for (int i = 0; i < ppu_cycles; i++) {
       auto res = ppu.step();
       if(res.nmi){
            Interrupt.emplace(Interrupt_type::NMI);
       }
       if(res.frame_complete){
           frame_finished = true;
       }
   }

   if(frame_finished){
       pixels = ppu.get_frame();
       render_frame();
       return 1;
   }
   return 0;

}

PPU Bus::get_ppu() {
    return ppu;
}

std::vector<uint8_t> Bus::get_pixels() {
    return pixels;
}


void Bus::render_frame(){
    SDL_Renderer *renderer = SDL_GetRenderer(window);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 256, 240);


    SDL_UpdateTexture(texture, NULL, pixels.data(), 256 * 3);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(texture);

    for(SDL_Event event; SDL_PollEvent(&event);){
        if(event.type == SDL_KEYDOWN){
            switch (event.key.keysym.sym){
                case SDLK_UP:
                    joypad1.set_button_pressed_state(UP, true);
                    break;
                case SDLK_DOWN:
                    joypad1.set_button_pressed_state(DOWN, true);
                    break;
                case SDLK_LEFT:
                    joypad1.set_button_pressed_state(LEFT, true);
                    break;
                case SDLK_RIGHT:
                    joypad1.set_button_pressed_state(RIGHT, true);
                    break;
                case SDLK_a:
                    joypad1.set_button_pressed_state(BUTTON_A, true);
                    break;
                case SDLK_s:
                    joypad1.set_button_pressed_state(BUTTON_B, true);
                    break;
                case SDLK_RETURN:
                    joypad1.set_button_pressed_state(START, true);
                    break;
                case SDLK_BACKSPACE:
                    joypad1.set_button_pressed_state(SELECT, true);
                    break;
            }
        }else if(event.type == SDL_KEYUP){
            switch (event.key.keysym.sym){
                case SDLK_UP:
                    joypad1.set_button_pressed_state(UP, false);
                    break;
                case SDLK_DOWN:
                    joypad1.set_button_pressed_state(DOWN, false);
                    break;
                case SDLK_LEFT:
                    joypad1.set_button_pressed_state(LEFT, false);
                    break;
                case SDLK_RIGHT:
                    joypad1.set_button_pressed_state(RIGHT, false);
                    break;
                case SDLK_a:
                    joypad1.set_button_pressed_state(BUTTON_A, false);
                    break;
                case SDLK_s:
                    joypad1.set_button_pressed_state(BUTTON_B, false);
                    break;
                case SDLK_RETURN:
                    joypad1.set_button_pressed_state(START, false);
                    break;
                case SDLK_BACKSPACE:
                    joypad1.set_button_pressed_state(SELECT, false);
                    break;
            }
        }
        if(event.type == SDL_QUIT){
            exit(0);
        }
    }

}




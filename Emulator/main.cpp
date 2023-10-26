#include <vector>
#include <SDL2/SDL.h>

#include <thread>
#include <fstream>
#include "CPU.h"
#include "Mappers/mapper0.h"
#include "emscripten.h"



std::string game = "game.nes";
uint32_t window_id;

void start_emulator();


EM_ASYNC_JS(int, load_game_preview, (), {

    const response = await fetch("game.nes").then(async response => {
        if(!response.ok){
            return -1;
        }
        return response.blob();

    });

    let data = new Uint8Array(await response.arrayBuffer());

    Module['FS_createDataFile'](".", "game.nes", data, true, true);
    return 0;
});




int main(){
    load_game_preview();
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("NES Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256 * 3, 240 * 3, SDL_WINDOW_SHOWN);
    window_id = SDL_GetWindowID(window);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetScale(renderer, 3, 3);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, 256, 240);




    start_emulator();

    return 0;
}







void start_emulator(){



    std::ifstream rom_file(game, std::ios::binary);

    std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(rom_file)), std::istreambuf_iterator<char>());
    rom_file.close();

    ROM rom = ROM(bytes);
    mapper0 mapper0(rom.get_prg_rom(),rom.get_chr_rom(),rom.get_screen_mirroring());
    Bus bus = Bus(&mapper0, SDL_GetWindowFromID(window_id));



    CPU cpu = CPU(&bus);
    cpu.reset();
    cpu.init_apu();



    emscripten_set_main_loop_arg(
            [](void *arg){
                CPU *cpu = (CPU *) arg;
                cpu->run();
            },&cpu,0,1);



}
extern "C"{
void load_game(std::string game_title){
        emscripten_cancel_main_loop();
        game = game_title;
        start_emulator();
}
}





extern "C"{
    void mount_FS(){

        EM_ASM(
                FS.mkdir('/games');
                // Then mount with IDBFS type
                FS.mount(IDBFS, {}, '/games');
                // Then sync
                FS.syncfs(true, function (err) {
            // Error
        });
        );




    }
}

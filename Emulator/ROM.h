#ifndef NES_EMULATOR_ROM_H
#define NES_EMULATOR_ROM_H


#include <cstdint>
#include "Mirroring.h"
#include <vector>
#include <iostream>

class ROM {
public:

    ROM(const std::vector<uint8_t> &data);
    ROM();
    ~ROM() = default;
    [[nodiscard]] std::vector<uint8_t > get_prg_rom() const {return prg_rom;}

    [[nodiscard]] std::vector<uint8_t> get_chr_rom() const {return chr_rom;}
    [[nodiscard]] uint8_t get_mapper() const {return mapper;}
    [[nodiscard]] mirroring get_screen_mirroring() const {return screen_mirroring;}
    [[nodiscard]] uint8_t get_prg_rom_size() const {return prg_rom.size();}
    [[nodiscard]] uint8_t get_chr_rom_size() const {return chr_rom.size();}

private:
    std::vector<uint8_t> prg_rom;
    std::vector<uint8_t> chr_rom;
    uint8_t mapper;
    mirroring screen_mirroring;
};


#endif //NES_EMULATOR_ROM_H

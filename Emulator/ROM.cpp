#include "ROM.h"
#define PRG_ROM_SIZE 0x4000
#define CHR_ROM_SIZE 0x2000


ROM::ROM() = default;


ROM::ROM(const std::vector<uint8_t> &data){
    if(!(data[0]==0x4E && data[1]==0x45 && data[2] == 0x53 && data[3] == 0x1A) ){
        throw std::runtime_error ("Invalid ROM");
    }

    mapper = (data[7] & 0xF0) | data[6] >> 4;

    uint8_t  ines_version = (data[7] >> 2) & 0b11;

    if(ines_version != 0){
        throw std::runtime_error ("NES 2.0 format is not supported");
    }


    if((data[6] & 0b1000) != 0){
        screen_mirroring = mirroring::FOUR_SCREEN;
    }else if ((data[6] & 0b1) != 0){
        screen_mirroring = mirroring::VERTICAL;
    }else{
        screen_mirroring = mirroring::HORIZONTAL;
    }

    size_t prg_rom_size = data[4] * PRG_ROM_SIZE;
    size_t chr_rom_size = data[5] * CHR_ROM_SIZE;

    bool skip_trainer = (data[6] & 0b100) != 0;

    size_t prg_rom_start = 16 + (skip_trainer ? 512 : 0);
    size_t chr_rom_start = prg_rom_start + prg_rom_size;

    prg_rom = std::vector<uint8_t>(&data[prg_rom_start], &data[prg_rom_start + prg_rom_size]);
    chr_rom = std::vector<uint8_t>(&data[chr_rom_start], &data[chr_rom_start + chr_rom_size]);



}
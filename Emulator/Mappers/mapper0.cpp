#include "mapper0.h"

mirroring mapper0::get_mirroring() {
    return mirroring_mode;
}

uint8_t mapper0::read(uint16_t address) {
    switch (address) {
        case 0x0000 ... 0x1FFF:
            return chr_rom[address % chr_rom.size()];
        case 0x6000 ... 0x7FFF:
                return prg_ram[address - 0x6000];
        case 0x8000 ... 0xFFFF:
            return prg_rom[address % prg_rom.size()];
        default:
            throw std::runtime_error("Invalid address");
    }

}

void mapper0::write(uint16_t address, uint8_t data) {
    switch (address) {
        case 0x0000 ... 0x1FFF:
            chr_rom[address % chr_rom.size()] = data;
            break;
        case 0x6000 ... 0x7FFF:
            prg_ram[address - 0x6000] = data;
            break;
        default:
            throw std::runtime_error("Invalid address");
    }
}

bool mapper0::irq_flag() {
    return false;
}


mapper0::mapper0(std::vector<uint8_t> prg_rom, std::vector<uint8_t> chr_rom, mirroring mirroring_mode) {

    this->prg_rom = std::move(prg_rom);
    this->chr_rom = std::move(chr_rom);
    this->mirroring_mode = mirroring::HORIZONTAL;
    this->mirroring_mode = mirroring_mode;

}


#ifndef NES_EMULATOR_MAPPER0_H
#define NES_EMULATOR_MAPPER0_H


#include <vector>
#include <cstdint>
#include <array>
#include "../Mapper.h"

class mapper0 : public Mapper {
    public:
    std::vector<uint8_t> chr_rom;
    std::vector<uint8_t> prg_rom;
    std::array<uint8_t, 0x2000> prg_ram;
    mirroring get_mirroring() override;
    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t data) override;
    bool irq_flag() override;
    mapper0(std::vector<uint8_t> prg_rom, std::vector<uint8_t> chr_rom, mirroring mirroring_mode);
    ~mapper0() = default;
};



#endif //NES_EMULATOR_MAPPER0_H

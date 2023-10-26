
#ifndef NES_EMULATOR_MAPPER_H
#define NES_EMULATOR_MAPPER_H

#include <cstdint>
#include <array>
#include "Mirroring.h"

class Mapper {




public:
    Mapper();
    ~Mapper() = default;
    virtual mirroring get_mirroring() = 0;
    virtual uint8_t read(uint16_t address) = 0;
    virtual void write(uint16_t address, uint8_t data) = 0;
    virtual bool irq_flag() = 0;

    std::array<int,4> coefficients();
    mirroring from_hv01(uint8_t mode);
    mirroring from_vh01(uint8_t mode);
    mirroring mirroring_mode;

};


#endif //NES_EMULATOR_MAPPER_H

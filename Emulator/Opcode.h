#ifndef NES_EMULATOR_OPCODE_H
#define NES_EMULATOR_OPCODE_H

#include <cstdint>
#include "addressing_mode.h"
#include <map>
#include <string>

class Opcode {

public:

    Opcode();

    Opcode(uint8_t opcode,std::string mnemonic, uint8_t bytes, uint8_t cycles, addressing_mode addressing_mode);

    ~Opcode();




    [[nodiscard]] uint8_t getOpcode() const;
    [[nodiscard]] uint8_t getBytes() const;
    [[nodiscard]] std::string getMnemonic() const;
    [[nodiscard]] uint8_t getCycles() const;
    [[nodiscard]] enum addressing_mode getAddressingMode() const;

    static std::map<uint8_t,Opcode> opcode_map;
private:
    uint8_t opcode;
    std::string mnemonic;
    uint8_t bytes;
    uint8_t cycles;
    enum addressing_mode addressing_mode;



};




#endif //NES_EMULATOR_OPCODE_H

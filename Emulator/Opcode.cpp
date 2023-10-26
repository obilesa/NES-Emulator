#include "Opcode.h"
#include "CPU.h"


Opcode::Opcode(uint8_t opcode,std::string mnemonic, uint8_t bytes, uint8_t cycles, enum addressing_mode addressing_mode) {
    this->opcode = opcode;
    this->mnemonic = mnemonic;
    this->bytes = bytes;
    this->cycles = cycles;
    this->addressing_mode = addressing_mode;
}

Opcode::Opcode() {
    this->opcode = 0;
    this->bytes = 0;
    this->cycles = 0;
    this->addressing_mode = addressing_mode::NONE_ADDRESSING;
}

Opcode::~Opcode() = default;


uint8_t Opcode::getOpcode() const {
    return opcode;
}

std::string Opcode::getMnemonic() const {
    return mnemonic;
}

uint8_t Opcode::getBytes() const {
    return bytes;
}

uint8_t Opcode::getCycles() const {
    return cycles;
}

enum addressing_mode Opcode::getAddressingMode() const {
    return addressing_mode;
}

std::map<uint8_t,Opcode> Opcode::opcode_map = {
        {0x00, Opcode(0x00, "BRK", 1, 7, addressing_mode::NONE_ADDRESSING)},




        {0x69, Opcode(0x69, "ADC", 2, 2, addressing_mode::IMMEDIATE)},
        {0x65, Opcode(0x65, "ADC", 2, 3, addressing_mode::ZERO_PAGE)},
        {0x75, Opcode(0x75, "ADC", 2, 4, addressing_mode::ZERO_PAGE_X)},
        {0x6d, Opcode(0x6d, "ADC", 3, 4, addressing_mode::ABSOLUTE)},
        {0x7d, Opcode(0x7d, "ADC", 3, 4, addressing_mode::ABSOLUTE_X)},
        {0x79, Opcode(0x79, "ADC", 3, 4, addressing_mode::ABSOLUTE_Y)},
        {0x61, Opcode(0x61, "ADC", 2, 6, addressing_mode::INDIRECT_X)},
        {0x71, Opcode(0x71, "ADC", 2, 5, addressing_mode::INDIRECT_Y)},

        {0x29, Opcode(0x29, "AND", 2, 2, addressing_mode::IMMEDIATE)},
        {0x25, Opcode(0x25, "AND", 2, 3, addressing_mode::ZERO_PAGE)},
        {0x35, Opcode(0x35, "AND", 2, 4, addressing_mode::ZERO_PAGE_X)},
        {0x2d, Opcode(0x2d, "AND", 3, 4, addressing_mode::ABSOLUTE)},
        {0x3d, Opcode(0x3d, "AND", 3, 4, addressing_mode::ABSOLUTE_X)},
        {0x39, Opcode(0x39, "AND", 3, 4, addressing_mode::ABSOLUTE_Y)},
        {0x21, Opcode(0x21, "AND", 2, 6, addressing_mode::INDIRECT_X)},
        {0x31, Opcode(0x31, "AND", 2, 5, addressing_mode::INDIRECT_Y)},

        {0x06, Opcode(0x06, "ASL", 2, 5, addressing_mode::ZERO_PAGE)},
        {0x16, Opcode(0x16, "ASL", 2, 6, addressing_mode::ZERO_PAGE_X)},
        {0x0e, Opcode(0x0e, "ASL", 3, 6, addressing_mode::ABSOLUTE)},
        {0x1e, Opcode(0x1e, "ASL", 3, 7, addressing_mode::ABSOLUTE_X)},
        {0x0a, Opcode(0x0a, "ASL", 1, 2, addressing_mode::NONE_ADDRESSING)},


        /*Branching*/
        {0x90, Opcode(0x90, "BCC", 2, 2, addressing_mode::RELATIVE)},

        {0xb0, Opcode(0xb0, "BCS", 2, 2, addressing_mode::RELATIVE)},

        {0xf0, Opcode(0xf0, "BEQ", 2, 2, addressing_mode::RELATIVE)},


        {0x24, Opcode(0x24, "BIT", 2, 3, addressing_mode::ZERO_PAGE)},
        {0x2c, Opcode(0x2c, "BIT", 3, 4, addressing_mode::ABSOLUTE)},

        {0x30, Opcode(0x30, "BMI", 2, 2, addressing_mode::RELATIVE)},

        {0xd0, Opcode(0xd0, "BNE", 2, 2, addressing_mode::RELATIVE)},

        {0x10, Opcode(0x10, "BPL", 2, 2, addressing_mode::RELATIVE)},

        {0x50, Opcode(0x50, "BVC", 2, 2, addressing_mode::RELATIVE)},

        {0x70, Opcode(0x70, "BVS", 2, 2, addressing_mode::RELATIVE)},

        // Clear flags
        {0x18, Opcode(0x18, "CLC", 1, 2, addressing_mode::NONE_ADDRESSING)},
        {0xd8, Opcode(0xd8, "CLD", 1, 2, addressing_mode::NONE_ADDRESSING)},
        {0x58, Opcode(0x58, "CLI", 1, 2, addressing_mode::NONE_ADDRESSING)},
        {0xb8, Opcode(0xb8, "CLV", 1, 2, addressing_mode::NONE_ADDRESSING)},


        {0xc9, Opcode(0xc9, "CMP", 2, 2, addressing_mode::IMMEDIATE)},
        {0xc5, Opcode(0xc5, "CMP", 2, 3, addressing_mode::ZERO_PAGE)},
        {0xd5, Opcode(0xd5, "CMP", 2, 4, addressing_mode::ZERO_PAGE_X)},
        {0xcd, Opcode(0xcd, "CMP", 3, 4, addressing_mode::ABSOLUTE)},
        {0xdd, Opcode(0xdd, "CMP", 3, 4, addressing_mode::ABSOLUTE_X)},
        {0xd9, Opcode(0xd9, "CMP", 3, 4, addressing_mode::ABSOLUTE_Y)},
        {0xc1, Opcode(0xc1, "CMP", 2, 6, addressing_mode::INDIRECT_X)},
        {0xd1, Opcode(0xd1, "CMP", 2, 5, addressing_mode::INDIRECT_Y)},

        {0xe0, Opcode(0xe0, "CPX", 2, 2, addressing_mode::IMMEDIATE)},
        {0xe4, Opcode(0xe4, "CPX", 2, 3, addressing_mode::ZERO_PAGE)},
        {0xec, Opcode(0xec, "CPX", 3, 4, addressing_mode::ABSOLUTE)},

        {0xc0, Opcode(0xc0, "CPY", 2, 2, addressing_mode::IMMEDIATE)},
        {0xc4, Opcode(0xc4, "CPY", 2, 3, addressing_mode::ZERO_PAGE)},
        {0xcc, Opcode(0xcc, "CPY", 3, 4, addressing_mode::ABSOLUTE)},

        {0xc6, Opcode(0xc6, "DEC", 2, 5, addressing_mode::ZERO_PAGE)},
        {0xd6, Opcode(0xd6, "DEC", 2, 6, addressing_mode::ZERO_PAGE_X)},
        {0xce, Opcode(0xce, "DEC", 3, 6, addressing_mode::ABSOLUTE)},
        {0xde, Opcode(0xde, "DEC", 3, 7, addressing_mode::ABSOLUTE_X)},

        {0xca, Opcode(0xca, "DEX", 1, 2, addressing_mode::NONE_ADDRESSING)},
        {0x88, Opcode(0x88, "DEY", 1, 2, addressing_mode::NONE_ADDRESSING)},

        {0x49, Opcode(0x49, "EOR", 2, 2, addressing_mode::IMMEDIATE)},
        {0x45, Opcode(0x45, "EOR", 2, 3, addressing_mode::ZERO_PAGE)},
        {0x55, Opcode(0x55, "EOR", 2, 4, addressing_mode::ZERO_PAGE_X)},
        {0x4d, Opcode(0x4d, "EOR", 3, 4, addressing_mode::ABSOLUTE)},
        {0x5d, Opcode(0x5d, "EOR", 3, 4, addressing_mode::ABSOLUTE_X)},
        {0x59, Opcode(0x59, "EOR", 3, 4, addressing_mode::ABSOLUTE_Y)},
        {0x41, Opcode(0x41, "EOR", 2, 6, addressing_mode::INDIRECT_X)},
        {0x51, Opcode(0x51, "EOR", 2, 5, addressing_mode::INDIRECT_Y)},

        {0xe6, Opcode(0xe6, "INC", 2, 5, addressing_mode::ZERO_PAGE)},
        {0xf6, Opcode(0xf6, "INC", 2, 6, addressing_mode::ZERO_PAGE_X)},
        {0xee, Opcode(0xee, "INC", 3, 6, addressing_mode::ABSOLUTE)},
        {0xfe, Opcode(0xfe, "INC", 3, 7, addressing_mode::ABSOLUTE_X)},

        {0xe8, Opcode(0xe8, "INX", 1, 2, addressing_mode::NONE_ADDRESSING)},
        {0xc8, Opcode(0xc8, "INY", 1, 2, addressing_mode::NONE_ADDRESSING)},

        {0x4c, Opcode(0x4c, "JMP", 3, 3, addressing_mode::ABSOLUTE)},
        {0x6c, Opcode(0x6c, "JMP", 3, 5, addressing_mode::INDIRECT)},

        {0x20, Opcode(0x20, "JSR", 3, 6, addressing_mode::ABSOLUTE)},

        {0xa9, Opcode(0xa9, "LDA", 2, 2, addressing_mode::IMMEDIATE)},
        {0xa5, Opcode(0xa5, "LDA", 2, 3, addressing_mode::ZERO_PAGE)},
        {0xb5, Opcode(0xb5, "LDA", 2, 4, addressing_mode::ZERO_PAGE_X)},
        {0xad, Opcode(0xad, "LDA", 3, 4, addressing_mode::ABSOLUTE)},
        {0xbd, Opcode(0xbd, "LDA", 3, 4, addressing_mode::ABSOLUTE_X)},
        {0xb9, Opcode(0xb9, "LDA", 3, 4, addressing_mode::ABSOLUTE_Y)},
        {0xa1, Opcode(0xa1, "LDA", 2, 6, addressing_mode::INDIRECT_X)},
        {0xb1, Opcode(0xb1, "LDA", 2, 5, addressing_mode::INDIRECT_Y)},

        {0xa2, Opcode(0xa2, "LDX", 2, 2, addressing_mode::IMMEDIATE)},
        {0xa6, Opcode(0xa6, "LDX", 2, 3, addressing_mode::ZERO_PAGE)},
        {0xb6, Opcode(0xb6, "LDX", 2, 4, addressing_mode::ZERO_PAGE_Y)},
        {0xae, Opcode(0xae, "LDX", 3, 4, addressing_mode::ABSOLUTE)},
        {0xbe, Opcode(0xbe, "LDX", 3, 4, addressing_mode::ABSOLUTE_Y)},

        {0xa0, Opcode(0xa0, "LDY", 2, 2, addressing_mode::IMMEDIATE)},
        {0xa4, Opcode(0xa4, "LDY", 2, 3, addressing_mode::ZERO_PAGE)},
        {0xb4, Opcode(0xb4, "LDY", 2, 4, addressing_mode::ZERO_PAGE_X)},
        {0xac, Opcode(0xac, "LDY", 3, 4, addressing_mode::ABSOLUTE)},
        {0xbc, Opcode(0xbc, "LDY", 3, 4, addressing_mode::ABSOLUTE_X)},

        {0x4a, Opcode(0x4a, "LSR", 1, 2, addressing_mode::NONE_ADDRESSING)},
        {0x46, Opcode(0x46, "LSR", 2, 5, addressing_mode::ZERO_PAGE)},
        {0x56, Opcode(0x56, "LSR", 2, 6, addressing_mode::ZERO_PAGE_X)},
        {0x4e, Opcode(0x4e, "LSR", 3, 6, addressing_mode::ABSOLUTE)},
        {0x5e, Opcode(0x5e, "LSR", 3, 7, addressing_mode::ABSOLUTE_X)},

        {0xea, Opcode(0xea, "NOP", 1, 2, addressing_mode::NONE_ADDRESSING)},

        {0x09, Opcode(0x09, "ORA", 2, 2, addressing_mode::IMMEDIATE)},
        {0x05, Opcode(0x05, "ORA", 2, 3, addressing_mode::ZERO_PAGE)},
        {0x15, Opcode(0x15, "ORA", 2, 4, addressing_mode::ZERO_PAGE_X)},
        {0x0d, Opcode(0x0d, "ORA", 3, 4, addressing_mode::ABSOLUTE)},
        {0x1d, Opcode(0x1d, "ORA", 3, 4, addressing_mode::ABSOLUTE_X)},
        {0x19, Opcode(0x19, "ORA", 3, 4, addressing_mode::ABSOLUTE_Y)},
        {0x01, Opcode(0x01, "ORA", 2, 6, addressing_mode::INDIRECT_X)},
        {0x11, Opcode(0x11, "ORA", 2, 5, addressing_mode::INDIRECT_Y)},

        {0x48, Opcode(0x48, "PHA", 1, 3, addressing_mode::NONE_ADDRESSING)},
        {0x08, Opcode(0x08, "PHP", 1, 3, addressing_mode::NONE_ADDRESSING)},
        {0x68, Opcode(0x68, "PLA", 1, 4, addressing_mode::NONE_ADDRESSING)},
        {0x28, Opcode(0x28, "PLP", 1, 4, addressing_mode::NONE_ADDRESSING)},

        {0x2a, Opcode(0x2a, "ROL", 1, 2, addressing_mode::NONE_ADDRESSING)},
        {0x26, Opcode(0x26, "ROL", 2, 5, addressing_mode::ZERO_PAGE)},
        {0x36, Opcode(0x36, "ROL", 2, 6, addressing_mode::ZERO_PAGE_X)},
        {0x2e, Opcode(0x2e, "ROL", 3, 6, addressing_mode::ABSOLUTE)},
        {0x3e, Opcode(0x3e, "ROL", 3, 7, addressing_mode::ABSOLUTE_X)},

        {0x6a, Opcode(0x6a, "ROR", 1, 2, addressing_mode::NONE_ADDRESSING)},
        {0x66, Opcode(0x66, "ROR", 2, 5, addressing_mode::ZERO_PAGE)},
        {0x76, Opcode(0x76, "ROR", 2, 6, addressing_mode::ZERO_PAGE_X)},
        {0x6e, Opcode(0x6e, "ROR", 3, 6, addressing_mode::ABSOLUTE)},
        {0x7e, Opcode(0x7e, "ROR", 3, 7, addressing_mode::ABSOLUTE_X)},

        {0x40, Opcode(0x40, "RTI", 1, 6, addressing_mode::NONE_ADDRESSING)},
        {0x60, Opcode(0x60, "RTS", 1, 6, addressing_mode::NONE_ADDRESSING)},

        {0xe9, Opcode(0xe9, "SBC", 2, 2, addressing_mode::IMMEDIATE)},
        {0xe5, Opcode(0xe5, "SBC", 2, 3, addressing_mode::ZERO_PAGE)},
        {0xf5, Opcode(0xf5, "SBC", 2, 4, addressing_mode::ZERO_PAGE_X)},
        {0xed, Opcode(0xed, "SBC", 3, 4, addressing_mode::ABSOLUTE)},
        {0xfd, Opcode(0xfd, "SBC", 3, 4, addressing_mode::ABSOLUTE_X)},
        {0xf9, Opcode(0xf9, "SBC", 3, 4, addressing_mode::ABSOLUTE_Y)},
        {0xe1, Opcode(0xe1, "SBC", 2, 6, addressing_mode::INDIRECT_X)},
        {0xf1, Opcode(0xf1, "SBC", 2, 5, addressing_mode::INDIRECT_Y)},

        {0x38, Opcode(0x38, "SEC", 1, 2, addressing_mode::NONE_ADDRESSING)},
        {0xf8, Opcode(0xf8, "SED", 1, 2, addressing_mode::NONE_ADDRESSING)},
        {0x78, Opcode(0x78, "SEI", 1, 2, addressing_mode::NONE_ADDRESSING)},

        {0x8d, Opcode(0x8d, "STA", 3, 4, addressing_mode::ABSOLUTE)},
        {0x9d, Opcode(0x9d, "STA", 3, 5, addressing_mode::ABSOLUTE_X)},
        {0x99, Opcode(0x99, "STA", 3, 5, addressing_mode::ABSOLUTE_Y)},
        {0x81, Opcode(0x81, "STA", 2, 6, addressing_mode::INDIRECT_X)},
        {0x91, Opcode(0x91, "STA", 2, 6, addressing_mode::INDIRECT_Y)},
        {0x85, Opcode(0x85, "STA", 2, 3, addressing_mode::ZERO_PAGE)},
        {0x95, Opcode(0x95, "STA", 2, 4, addressing_mode::ZERO_PAGE_X)},

        {0x86, Opcode(0x86, "STX", 2, 3, addressing_mode::ZERO_PAGE)},
        {0x96, Opcode(0x96, "STX", 2, 4, addressing_mode::ZERO_PAGE_Y)},
        {0x8e, Opcode(0x8e, "STX", 3, 4, addressing_mode::ABSOLUTE)},

        {0x84, Opcode(0x84, "STY", 2, 3, addressing_mode::ZERO_PAGE)},
        {0x94, Opcode(0x94, "STY", 2, 4, addressing_mode::ZERO_PAGE_X)},
        {0x8c, Opcode(0x8c, "STY", 3, 4, addressing_mode::ABSOLUTE)},

        {0xaa, Opcode(0xaa, "TAX", 1, 2, addressing_mode::NONE_ADDRESSING)},
        {0xa8, Opcode(0xa8, "TAY", 1, 2, addressing_mode::NONE_ADDRESSING)},
        {0xba, Opcode(0xba, "TSX", 1, 2, addressing_mode::NONE_ADDRESSING)},
        {0x8a, Opcode(0x8a, "TXA", 1, 2, addressing_mode::NONE_ADDRESSING)},
        {0x9a, Opcode(0x9a, "TXS", 1, 2, addressing_mode::NONE_ADDRESSING)},
        {0x98, Opcode(0x98, "TYA", 1, 2, addressing_mode::NONE_ADDRESSING)},







        {0x0b, Opcode(0x0b,"AAC",2, 2, addressing_mode::IMMEDIATE)},
        {0x2b, Opcode(0x2b,"AAC", 2, 2, addressing_mode::IMMEDIATE)},

        {0x87, Opcode(0x87,"AAX", 2, 3, addressing_mode::ZERO_PAGE)},
        {0x97, Opcode(0x97,"AAX", 2, 4, addressing_mode::ZERO_PAGE_Y)},
        {0x83, Opcode(0x83,"AAX", 2, 6, addressing_mode::INDIRECT_X)},
        {0x8f, Opcode(0x8f,"AAX", 3, 4, addressing_mode::ABSOLUTE)},

        {0x6b, Opcode(0x6b, "ARR", 2, 2, addressing_mode::IMMEDIATE)},

        {0x4b, Opcode(0x4b, "ASR", 2, 2, addressing_mode::IMMEDIATE)},

        {0xab, Opcode(0xab, "ATX", 2, 2, addressing_mode::IMMEDIATE)},

        {0x9f, Opcode(0x9f, "AXA", 3, 5, addressing_mode::ABSOLUTE_Y)},
        {0x93, Opcode(0x93, "AXA", 2, 6, addressing_mode::INDIRECT_Y)},

        {0xcb, Opcode(0xcb, "AXS", 2, 2, addressing_mode::IMMEDIATE)},

        {0xc7, Opcode(0xc7, "DCP", 2, 5, addressing_mode::ZERO_PAGE)},
        {0xd7, Opcode(0xd7, "DCP", 2, 6, addressing_mode::ZERO_PAGE_X)},
        {0xcf, Opcode(0xcf, "DCP", 3, 6, addressing_mode::ABSOLUTE)},
        {0xdf, Opcode(0xdf, "DCP", 3, 7, addressing_mode::ABSOLUTE_X)},
        {0xdb, Opcode(0xdb, "DCP", 3, 7, addressing_mode::ABSOLUTE_Y)},
        {0xc3, Opcode(0xc3, "DCP", 2, 8, addressing_mode::INDIRECT_X)},
        {0xd3, Opcode(0xd3, "DCP", 2, 8, addressing_mode::INDIRECT_Y)},

        {0x04, Opcode(0x04, "DOP", 2, 3, addressing_mode::ZERO_PAGE)},
        {0x14, Opcode(0x14, "DOP", 2, 4, addressing_mode::ZERO_PAGE_X)},
        {0x34, Opcode(0x34, "DOP", 2, 4, addressing_mode::ZERO_PAGE_X)},
        {0x44, Opcode(0x44, "DOP", 2, 3, addressing_mode::ZERO_PAGE)},
        {0x54, Opcode(0x54, "DOP", 2, 4, addressing_mode::ZERO_PAGE_X)},
        {0x64, Opcode(0x64, "DOP", 2, 3, addressing_mode::ZERO_PAGE)},
        {0x74, Opcode(0x74, "DOP", 2, 4, addressing_mode::ZERO_PAGE_X)},
        {0x80, Opcode(0x80, "DOP", 2, 2, addressing_mode::IMMEDIATE)},
        {0x82, Opcode(0x82, "DOP", 2, 2, addressing_mode::IMMEDIATE)},
        {0x89, Opcode(0x89, "DOP", 2, 2, addressing_mode::IMMEDIATE)},
        {0xc2, Opcode(0xc2, "DOP", 2, 2, addressing_mode::IMMEDIATE)},
        {0xd4, Opcode(0xd4, "DOP", 2, 4, addressing_mode::ZERO_PAGE_X)},
        {0xe2, Opcode(0xe2, "DOP", 2, 2, addressing_mode::IMMEDIATE)},
        {0xf4, Opcode(0xf4, "DOP", 2, 4, addressing_mode::ZERO_PAGE_X)},

        {0xe7, Opcode(0xe7, "ISC",2,5,addressing_mode::ZERO_PAGE)},
        {0xf7, Opcode(0xf7, "ISC",2,6,addressing_mode::ZERO_PAGE_X)},
        {0xef, Opcode(0xef, "ISC",3,6,addressing_mode::ABSOLUTE)},
        {0xff, Opcode(0xff, "ISC",3,7,addressing_mode::ABSOLUTE_X)},
        {0xfb, Opcode(0xfb, "ISC",3,7,addressing_mode::ABSOLUTE_Y)},
        {0xe3, Opcode(0xe3, "ISC",2,8,addressing_mode::INDIRECT_X)},
        {0xf3, Opcode(0xf3, "ISC",2,8,addressing_mode::INDIRECT_Y)},

        {0x02, Opcode(0x02, "KIL",1,0,addressing_mode::NONE_ADDRESSING)},
        {0x12, Opcode(0x12, "KIL",1,0,addressing_mode::NONE_ADDRESSING)},
        {0x22, Opcode(0x22, "KIL",1,0,addressing_mode::NONE_ADDRESSING)},
        {0x32, Opcode(0x32, "KIL",1,0,addressing_mode::NONE_ADDRESSING)},
        {0x42, Opcode(0x42, "KIL",1,0,addressing_mode::NONE_ADDRESSING)},
        {0x52, Opcode(0x52, "KIL",1,0,addressing_mode::NONE_ADDRESSING)},
        {0x62, Opcode(0x62, "KIL",1,0,addressing_mode::NONE_ADDRESSING)},
        {0x72, Opcode(0x72, "KIL",1,0,addressing_mode::NONE_ADDRESSING)},
        {0x92, Opcode(0x92, "KIL",1,0,addressing_mode::NONE_ADDRESSING)},
        {0xb2, Opcode(0xb2, "KIL",1,0,addressing_mode::NONE_ADDRESSING)},
        {0xd2, Opcode(0xd2, "KIL",1,0,addressing_mode::NONE_ADDRESSING)},
        {0xf2, Opcode(0xf2, "KIL",1,0,addressing_mode::NONE_ADDRESSING)},

        {0xbb, Opcode(0xbb, "LAR",3,4,addressing_mode::ABSOLUTE_Y)},

        {0xa7, Opcode(0xa7, "LAX",2,3,addressing_mode::ZERO_PAGE)},
        {0xb7, Opcode(0xb7, "LAX",2,4,addressing_mode::ZERO_PAGE_Y)},
        {0xaf, Opcode(0xaf, "LAX",3,4,addressing_mode::ABSOLUTE)},
        {0xbf, Opcode(0xbf, "LAX",3,4,addressing_mode::ABSOLUTE_Y)},
        {0xa3, Opcode(0xa3, "LAX",2,6,addressing_mode::INDIRECT_X)},
        {0xb3, Opcode(0xb3, "LAX",2,5,addressing_mode::INDIRECT_Y)},

        {0x1a, Opcode(0x1a, "NOP",1,2,addressing_mode::NONE_ADDRESSING)},
        {0x3a, Opcode(0x3a, "NOP",1,2,addressing_mode::NONE_ADDRESSING)},
        {0x5a, Opcode(0x5a, "NOP",1,2,addressing_mode::NONE_ADDRESSING)},
        {0x7a, Opcode(0x7a, "NOP",1,2,addressing_mode::NONE_ADDRESSING)},
        {0xda, Opcode(0xda, "NOP",1,2,addressing_mode::NONE_ADDRESSING)},
        {0xfa, Opcode(0xfa, "NOP",1,2,addressing_mode::NONE_ADDRESSING)},

        {0x27, Opcode(0x27, "RLA",2,5,addressing_mode::ZERO_PAGE)},
        {0x37, Opcode(0x37, "RLA",2,6,addressing_mode::ZERO_PAGE_X)},
        {0x2f, Opcode(0x2f, "RLA",3,6,addressing_mode::ABSOLUTE)},
        {0x3f, Opcode(0x3f, "RLA",3,7,addressing_mode::ABSOLUTE_X)},
        {0x3b, Opcode(0x3b, "RLA",3,7,addressing_mode::ABSOLUTE_Y)},
        {0x23, Opcode(0x23, "RLA",2,8,addressing_mode::INDIRECT_X)},
        {0x33, Opcode(0x33, "RLA",2,8,addressing_mode::INDIRECT_Y)},

        {0x67, Opcode(0x67, "RRA",2,5,addressing_mode::ZERO_PAGE)},
        {0x77, Opcode(0x77, "RRA",2,6,addressing_mode::ZERO_PAGE_X)},
        {0x6f, Opcode(0x6f, "RRA",3,6,addressing_mode::ABSOLUTE)},
        {0x7f, Opcode(0x7f, "RRA",3,7,addressing_mode::ABSOLUTE_X)},
        {0x7b, Opcode(0x7b, "RRA",3,7,addressing_mode::ABSOLUTE_Y)},
        {0x63, Opcode(0x63, "RRA",2,8,addressing_mode::INDIRECT_X)},
        {0x73, Opcode(0x73, "RRA",2,8,addressing_mode::INDIRECT_Y)},

        {0xeb, Opcode(0xeb, "SBC",2,2,addressing_mode::IMMEDIATE)},

        {0x07, Opcode(0x07, "SLO",2,5,addressing_mode::ZERO_PAGE)},
        {0x17, Opcode(0x17, "SLO",2,6,addressing_mode::ZERO_PAGE_X)},
        {0x0f, Opcode(0x0f, "SLO",3,6,addressing_mode::ABSOLUTE)},
        {0x1f, Opcode(0x1f, "SLO",3,7,addressing_mode::ABSOLUTE_X)},
        {0x1b, Opcode(0x1b, "SLO",3,7,addressing_mode::ABSOLUTE_Y)},
        {0x03, Opcode(0x03, "SLO",2,8,addressing_mode::INDIRECT_X)},
        {0x13, Opcode(0x13, "SLO",2,8,addressing_mode::INDIRECT_Y)},

        {0x47, Opcode(0x47, "SRE",2,5,addressing_mode::ZERO_PAGE)},
        {0x57, Opcode(0x57, "SRE",2,6,addressing_mode::ZERO_PAGE_X)},
        {0x4f, Opcode(0x4f, "SRE",3,6,addressing_mode::ABSOLUTE)},
        {0x5f, Opcode(0x5f, "SRE",3,7,addressing_mode::ABSOLUTE_X)},
        {0x5b, Opcode(0x5b, "SRE",3,7,addressing_mode::ABSOLUTE_Y)},
        {0x43, Opcode(0x43, "SRE",2,8,addressing_mode::INDIRECT_X)},
        {0x53, Opcode(0x53, "SRE",2,8,addressing_mode::INDIRECT_Y)},

        {0x9e, Opcode(0x9e, "SXA",3,5,addressing_mode::ABSOLUTE_Y)},
        {0x9c, Opcode(0x9c, "SYA",3,5,addressing_mode::ABSOLUTE_X)},

        {0x0c, Opcode(0x0c,"TOP",3,4,addressing_mode::ABSOLUTE)},
        {0x1c, Opcode(0x1c,"TOP",3,4,addressing_mode::ABSOLUTE_X)},
        {0x3c, Opcode(0x3c,"TOP",3,4,addressing_mode::ABSOLUTE_X)},
        {0x5c, Opcode(0x5c,"TOP",3,4,addressing_mode::ABSOLUTE_X)},
        {0x7c, Opcode(0x7c,"TOP",3,4,addressing_mode::ABSOLUTE_X)},
        {0xdc, Opcode(0xdc,"TOP",3,4,addressing_mode::ABSOLUTE_X)},
        {0xfc, Opcode(0xfc,"TOP",3,4,addressing_mode::ABSOLUTE_X)},

        {0x8b, Opcode(0x8b,"XAA",2,2,addressing_mode::IMMEDIATE)},

        {0x9b, Opcode(0x9b,"XAS",3,5,addressing_mode::ABSOLUTE_Y)},



        };






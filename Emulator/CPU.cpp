//#include <gtest/gtest.h>

#include <iomanip>
#include "CPU.h"


// Flags
#define CARRY_FLAG 0b00000001
#define ZERO_FLAG 0b00000010
#define INTERRUPT_FLAG 0b00000100
#define DECIMAL_FLAG 0b00001000
#define BREAK_FLAG 0b00010000
#define BREAK_FLAG_2 0b00100000
#define OVERFLOW_FLAG 0b01000000
#define NEGATIVE_FLAG 0b10000000


// Stack
#define STACK_START 0x0100



CPU::CPU() {
    register_a = 0;
    register_x = 0;
    register_y = 0;
    status_register = 0b00110110;
    program_counter = 0;
    stack_pointer = 0xFD;
    memory.fill(0);
    Bus _bus = Bus();
    bus =  &_bus;
}

CPU::CPU(Bus *_bus) {
    register_a = 0;
    register_x = 0;
    register_y = 0;
    status_register = 0b00110110;
    program_counter = 0;
    stack_pointer = 0xFD;
    memory.fill(0);
    bus = _bus;
}

CPU::~CPU() = default;



uint8_t CPU::get_register_a() const {
    return register_a;
}

uint8_t CPU::get_register_x() const {
    return register_x;
}

uint8_t CPU::get_register_y() const {
    return register_y;
}

uint8_t CPU::get_stack_pointer() const {
    return stack_pointer;
}

uint8_t CPU::get_status_register() const {
    return status_register;
}

uint16_t CPU::get_program_counter() const {
    return program_counter;
}
void CPU::set_program_counter(uint16_t value) {
    program_counter = value;
}

uint8_t CPU::read_memory(uint16_t address) {
    if(address == apu.status_addr){
        return apu.read_status(elapsed_cycles);
    }
    return bus->read_memory(address);
}

void CPU::write_memory(uint16_t address, uint8_t value) {
    if(address >= apu.start_addr && address <= apu.end_addr){
        apu.write_register(elapsed_cycles, address, value);
    }
    bus->write_memory(address, value);
}

uint16_t CPU::read_memory_16(uint16_t address) {
    uint16_t low_byte = read_memory(address);
    uint16_t high_byte = read_memory(address + 1);
    return (high_byte << 8) | low_byte;
}

void CPU::write_memory_16(uint16_t address, uint16_t value) {
    uint8_t low_byte = value & 0xFF;
    uint8_t high_byte = (value >> 8);
    CPU::write_memory(address, low_byte);
    CPU::write_memory(address + 1, high_byte);
}

uint8_t CPU::stack_pop() {
    CPU::stack_pointer++;
    return read_memory(STACK_START + CPU::stack_pointer);
}

void CPU::stack_push(uint8_t value) {
    write_memory(STACK_START + CPU::stack_pointer, value);
    stack_pointer--;
}

void CPU::stack_push_16(uint16_t value) {
    uint8_t high_byte = (value >> 8);
    uint8_t low_byte = value & 0xFF;
    CPU::stack_push(high_byte);
    CPU::stack_push(low_byte);
}

uint16_t CPU::stack_pop_16() {
    uint16_t low_byte = CPU::stack_pop();
    uint16_t high_byte = CPU::stack_pop();
    return (high_byte << 8) | low_byte;
}


void CPU::reset() {
    CPU::register_a = 0;
    CPU::register_x = 0;
    CPU::register_y = 0;
    CPU::status_register = 0b100100;
    CPU::stack_pointer = 0xFD;
    CPU::program_counter = read_memory_16(0xFFFC);
    bus->Interrupt = std::nullopt;

}

uint16_t CPU::get_operand_address(enum addressing_mode mode) {
    switch (mode) {
        case addressing_mode::IMMEDIATE:
            return CPU::program_counter;
        case addressing_mode::ZERO_PAGE:
            return CPU::read_memory(CPU::program_counter);
        case addressing_mode::ZERO_PAGE_X:
            return (uint8_t) (CPU::read_memory(program_counter) + register_x);
        case addressing_mode::ZERO_PAGE_Y:
            return (uint8_t) (CPU::read_memory(program_counter) + register_y);
        case addressing_mode::ABSOLUTE:
            return CPU::read_memory_16(CPU::program_counter);
        case addressing_mode::ABSOLUTE_X:
            return (uint16_t) (read_memory_16(CPU::program_counter) + CPU::register_x);
        case addressing_mode::ABSOLUTE_Y:
            return (uint16_t) (read_memory_16(CPU::program_counter) + CPU::register_y);
        case addressing_mode::INDIRECT:
            if (CPU::read_memory(CPU::program_counter) == 0xFF) {
                uint8_t low_byte = CPU::read_memory(CPU::read_memory_16(CPU::program_counter));
                uint8_t high_byte = CPU::read_memory(CPU::read_memory_16(CPU::program_counter) & 0xFF00);
                return (uint16_t(high_byte) << 8) | uint16_t(low_byte);
            } else {
                return CPU::read_memory_16(CPU::read_memory_16(CPU::program_counter));
            }
        case addressing_mode::INDIRECT_X: {
            uint8_t base_address = CPU::read_memory(CPU::program_counter);
            uint8_t pointer_address = base_address + CPU::register_x ;
            uint8_t low_byte = CPU::read_memory(pointer_address);
            uint8_t high_byte = CPU::read_memory((uint8_t) (pointer_address + 1));
            return (high_byte << 8) | low_byte;
        }
        case addressing_mode::INDIRECT_Y: {
            uint8_t base_address = CPU::read_memory(CPU::program_counter);
            uint8_t low_byte = CPU::read_memory(base_address);
            uint8_t high_byte = CPU::read_memory((uint8_t) (base_address + 1));
            uint16_t pointer_address = uint16_t(high_byte) << 8 | low_byte;
            return (uint16_t) (pointer_address + CPU::register_y) ;
        }
        case addressing_mode::NONE_ADDRESSING:
            return 0;
        default:
            return -1;
    }
}


bool CPU::page_crossed(addressing_mode addressing_mode){
    switch (addressing_mode) {
        case addressing_mode::ABSOLUTE_X:
            return (read_memory_16(program_counter) & 0xFF00) !=
                   ((read_memory_16(program_counter) + register_x) & 0xFF00);
        case addressing_mode::ABSOLUTE_Y:
            return (read_memory_16(program_counter) & 0xFF00) !=
                   ((read_memory_16(program_counter) + register_y) & 0xFF00);
        case addressing_mode::INDIRECT_Y: {
            uint8_t base_address = CPU::read_memory(CPU::program_counter);
            uint8_t pointer_address = read_memory(base_address) >> 8 | read_memory((uint8_t) (base_address + 1));
            return (pointer_address & 0xFF00) != ((pointer_address + register_y) & 0xFF00);
        }
        default:
            return false;
    }
}

bool CPU::page_crossed(uint16_t address1 , uint16_t address2){
    return (address1 & 0xFF00) != (address2 & 0xFF00);
}


void CPU::fetch_and_exectute(std::vector<uint8_t> program) {
    CPU::load(std::move(program));
    CPU::reset();
    CPU::run();
}

void CPU::load(std::vector<uint8_t> program) {
    std::copy(program.begin(), program.end(), CPU::memory.begin() + 0x600);
    write_memory_16(0xFFFC, 0x600);
}

void CPU::tax() {
    CPU::register_x = CPU::register_a;
    zero_flag(CPU::register_x);
    negative_flag(CPU::register_x);
}


void CPU::lda(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    register_a = read_memory(address);
    zero_flag(CPU::register_a);
    negative_flag(CPU::register_a);

    if(page_crossed(mode)){
        elapsed_cycles++;
        bus->tick(1);
    }
}

void CPU::sta(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    write_memory(address, CPU::register_a);

}

/*
 * TODO: More tests are needed
 */
void CPU::adc(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    uint16_t result = register_a + read_memory(address) + (status_register & CARRY_FLAG);
    carry_flag(result > 0xFF);
    overflow_flag(((register_a ^ result) & 0x80) && !((register_a ^ read_memory(address)) & 0x80));
    register_a = result & 0xFF;
    zero_flag(register_a);
    negative_flag(register_a);

    if(page_crossed(mode)){
        elapsed_cycles++;
        bus->tick(1);
    }


}

void CPU::sbc(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    uint16_t result = register_a - read_memory(address) - (1 - (status_register & CARRY_FLAG));
    carry_flag(result <= 0xFF);
    overflow_flag(((register_a ^ result) & 0x80) & ((register_a ^ read_memory(address)) & 0x80));
    register_a = result & 0xFF;
    zero_flag(register_a);
    negative_flag(register_a);

    if(page_crossed(mode)){
        elapsed_cycles++;
        bus->tick(1);
    }

}

void CPU::logical_and(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    register_a = register_a & read_memory(address);
    zero_flag(register_a);
    negative_flag(register_a);

    if(page_crossed(mode)){
        elapsed_cycles++;
        bus->tick(1);
    }
}

void CPU::asl(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    uint8_t value = read_memory(address);
    carry_flag((value & 0b10000000) > 0);
    value = value << 1;
    write_memory(address, value);
    zero_flag(value);
    negative_flag(value);
}

void CPU::asl_accumulator() {
    carry_flag((register_a & 0b10000000) > 0);
    register_a = register_a << 1;
    zero_flag(register_a);
    negative_flag(register_a);
}

void CPU::
bcc() {
    if ((status_register & CARRY_FLAG) == 0) {
        program_counter += int8_t(read_memory(program_counter)) + 1;
    }
}

void CPU::bcs() {
    if ((status_register & CARRY_FLAG) > 0) {
        program_counter += int8_t(read_memory(program_counter)) + 1;
    }
}

void CPU::beq() {
    if ((status_register & ZERO_FLAG) > 0) {
        elapsed_cycles++;
        bus->tick(1);
        auto offset = (int8_t) (read_memory(program_counter) +1);
        if(page_crossed(program_counter+1, program_counter + offset)){
            elapsed_cycles++;
            bus->tick(1);
        }
        program_counter += offset;
    }
}

void CPU::bit(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    uint8_t value = read_memory(address);
    zero_flag(register_a & value);
    overflow_flag((value & 0b01000000) > 0);
    negative_flag(value);
}

void CPU::bmi() {
    if ((status_register & NEGATIVE_FLAG) > 0) {
        elapsed_cycles++;
        bus->tick(1);
        auto offset = (int8_t) (read_memory(program_counter) +1);
        if(page_crossed(program_counter+1, program_counter + offset)){
            elapsed_cycles++;
            bus->tick(1);
        }
        program_counter += offset;
    }
}

void CPU::bne() {
    if ((status_register & ZERO_FLAG) == 0) {
        elapsed_cycles++;
        bus->tick(1);
        auto offset = (int8_t) (read_memory(program_counter) +1);
        if(page_crossed(program_counter+1, program_counter + offset)){
            elapsed_cycles++;
            bus->tick(1);
        }
        program_counter += offset;
    }
}

void CPU::bpl() {
    if ((status_register & NEGATIVE_FLAG) == 0) {
        elapsed_cycles++;
        bus->tick(1);
        auto offset = (int8_t) (read_memory(program_counter) +1);
        if(page_crossed(program_counter+1, program_counter + offset)){
            elapsed_cycles++;
            bus->tick(1);
        }
        program_counter += offset;
    }
}

// TODO: Implement BRK instruction

void CPU::bvc() {
    if ((status_register & OVERFLOW_FLAG) == 0) {
        elapsed_cycles++;
        bus->tick(1);
        auto offset = (int8_t) (read_memory(program_counter) +1);
        if(page_crossed(program_counter+1, program_counter + offset)){
            elapsed_cycles++;
            bus->tick(1);
        }
        program_counter += offset;
    }
}

void CPU::bvs() {
    if ((status_register & OVERFLOW_FLAG) > 0) {
        elapsed_cycles++;
        bus->tick(1);
        auto offset = (int8_t) (read_memory(program_counter) +1);
        if(page_crossed(program_counter+1, program_counter + offset)){
            elapsed_cycles++;
            bus->tick(1);
        }
        program_counter += offset;
    }
}

void CPU::clc() {
    carry_flag(false);
}

void CPU::cld() {
    status_register = status_register & ~DECIMAL_FLAG;
}

void CPU::cli() {
    status_register = status_register & ~INTERRUPT_FLAG;
}

void CPU::clv() {
    status_register = status_register & ~OVERFLOW_FLAG;
}

void CPU::cmp(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    uint8_t value = read_memory(address);
    carry_flag(register_a >= value);
    zero_flag(register_a - value);
    negative_flag(register_a - value);

    if(page_crossed(mode)){
        elapsed_cycles++;
        bus->tick(1);
    }

}

void CPU::cpx(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    uint8_t value = read_memory(address);
    carry_flag(register_x >= value);
    zero_flag(register_x - value);
    negative_flag(register_x - value);
}

void CPU::cpy(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    uint8_t value = read_memory(address);
    carry_flag(register_y >= value);
    zero_flag(register_y - value);
    negative_flag(register_y - value);
}

void CPU::dec(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    uint8_t value = read_memory(address);
    value--;
    write_memory(address, value);
    zero_flag(value);
    negative_flag(value);
}

void CPU::dex() {
    register_x--;
    zero_flag(register_x);
    negative_flag(register_x);
}

void CPU::dey() {
    register_y--;
    zero_flag(register_y);
    negative_flag(register_y);
}

void CPU::eor(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    register_a = register_a ^ read_memory(address);
    zero_flag(register_a);
    negative_flag(register_a);

    if(page_crossed(mode)){
        elapsed_cycles++;
        bus->tick(1);
    }
}

void CPU::inc(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    uint8_t value = read_memory(address);
    value++;
    write_memory(address, value);
    zero_flag(value);
    negative_flag(value);
}


void CPU::inx() {
    CPU::register_x++;
    zero_flag(CPU::register_x);
    negative_flag(CPU::register_x);
}

void CPU::iny() {
    register_y++;
    zero_flag(register_y);
    negative_flag(register_y);
}

void CPU::jmp(enum addressing_mode mode) {
    program_counter = get_operand_address(mode);
}

void CPU::jsr() {
    stack_push_16(program_counter + 2 - 1);
    program_counter = read_memory_16(program_counter);
}

void CPU::ldx(enum addressing_mode mode) {
    register_x = read_memory(get_operand_address(mode));
    zero_flag(register_x);
    negative_flag(register_x);

    if(page_crossed(mode)) {
        elapsed_cycles++;
        bus->tick(1);
    }
}

void CPU::ldy(enum addressing_mode mode) {
    register_y = read_memory(get_operand_address(mode));
    zero_flag(register_y);
    negative_flag(register_y);
    if(page_crossed(mode)) {
        elapsed_cycles++;
        bus->tick(1);
    }
}

void CPU::lsr_accumulator() {
    carry_flag((register_a & 0b00000001) > 0);
    register_a = register_a >> 1;
    zero_flag(register_a);
    negative_flag(register_a);
}

void CPU::lsr(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    uint8_t value = read_memory(address);
    carry_flag((value & 0b00000001) > 0);
    value = value >> 1;
    write_memory(address, value);
    zero_flag(value);
    negative_flag(value);
}

void CPU::nop() {
    // Do nothing
}

void CPU::ora(enum addressing_mode mode) {
    register_a = register_a | read_memory(get_operand_address(mode));
    zero_flag(register_a);
    negative_flag(register_a);

    if(page_crossed(mode)){
        elapsed_cycles++;
        bus->tick(1);
    }
}

void CPU::pha() {
    stack_push(register_a);
}

void CPU::php() {
    stack_push(status_register | BREAK_FLAG | BREAK_FLAG_2);
}

void CPU::pla() {
    register_a = stack_pop();
    zero_flag(register_a);
    negative_flag(register_a);
}

void CPU::plp() {
    status_register = stack_pop();
    status_register = status_register & ~BREAK_FLAG;
    status_register = status_register | BREAK_FLAG_2;
}

void CPU::rol_accumulator() {
    bool carry = (register_a & 0b10000000) > 0;
    register_a = register_a << 1;
    register_a = status_register & CARRY_FLAG ? register_a | CARRY_FLAG : register_a & ~CARRY_FLAG;
    carry_flag(carry);
    zero_flag(register_a);
    negative_flag(register_a);
}

void CPU::rol(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    uint8_t value = read_memory(address);
    bool carry = (value & 0b10000000) > 0;
    value = value << 1;
    value = status_register & CARRY_FLAG ? value | CARRY_FLAG : value & ~CARRY_FLAG;
    write_memory(address, value);
    carry_flag(carry);
    zero_flag(value);
    negative_flag(value);

}

void CPU::ror_accumulator() {
    bool carry = (register_a & 0b00000001) > 0;
    register_a = register_a >> 1;
    register_a = (status_register & CARRY_FLAG) ? (register_a | 0b10000000) : register_a & ~0b10000000;
    carry_flag(carry);
    zero_flag(register_a);
    negative_flag(register_a);
}

void CPU::ror(enum addressing_mode mode) {
    uint16_t address = get_operand_address(mode);
    uint8_t value = read_memory(address);
    bool carry = (value & 0b00000001) > 0;
    value = value >> 1;
    value = (status_register & CARRY_FLAG) ? (value | 0b10000000) : value & ~0b10000000;
    write_memory(address, value);
    carry_flag(carry);
    zero_flag(value);
    negative_flag(value);
}

void CPU::rti() {
    status_register = stack_pop();
    status_register = status_register & ~BREAK_FLAG;
    status_register = status_register | BREAK_FLAG_2;
    program_counter = stack_pop_16();
}

void CPU::rts() {
    program_counter = stack_pop_16() + 1;
}

void CPU::sec() {
    carry_flag(true);
}

void CPU::sed() {
    status_register = status_register | DECIMAL_FLAG;
}

void CPU::sei() {
    status_register = status_register | INTERRUPT_FLAG;
}

void CPU::stx(enum addressing_mode mode) {
    write_memory(get_operand_address(mode), register_x);
}

void CPU::sty(enum addressing_mode mode) {
    write_memory(get_operand_address(mode), register_y);
}

void CPU::tay() {
    register_y = register_a;
    zero_flag(register_y);
    negative_flag(register_y);
}

void CPU::tsx() {
    register_x = stack_pointer;
    zero_flag(register_x);
    negative_flag(register_x);
}

void CPU::txa() {
    register_a = register_x;
    zero_flag(register_a);
    negative_flag(register_a);
}

void CPU::txs() {
    stack_pointer = register_x;
}

void CPU::tya() {
    register_a = register_y;
    zero_flag(register_a);
    negative_flag(register_a);
}


void CPU::aac(enum addressing_mode mode){
    register_a = register_a & read_memory(get_operand_address(mode));
    carry_flag((register_a & NEGATIVE_FLAG) > 0);
    zero_flag(register_a);
    negative_flag(register_a);
}

void CPU::aax(enum addressing_mode mode){
    write_memory(get_operand_address(mode), register_a & register_x);
}

void CPU::arr(enum addressing_mode mode){
    register_a = register_a & read_memory(get_operand_address(mode));
    ror_accumulator();
    carry_flag((register_a & 0b01000000) > 0);
    overflow_flag(((register_a & 0b00100000) ^ ((register_a & 0b01000000) << 1) ) > 0);
    zero_flag(register_a);
    negative_flag(register_a);
}

void CPU::asr(enum addressing_mode mode){
    register_a = register_a & read_memory(get_operand_address(mode));
    lsr_accumulator();
}

void CPU::atx(enum addressing_mode mode){
    register_x = register_a = register_a & read_memory(get_operand_address(mode));
    zero_flag(register_a);
    negative_flag(register_a);
}

void CPU::axa(enum addressing_mode mode){
    uint16_t address = get_operand_address(mode);
    write_memory(address, (register_a & register_x) & (address >> 8));
}

void CPU::axs(enum addressing_mode mode){
    register_x = register_a & register_x;
    uint8_t value = register_x - read_memory(get_operand_address(mode));
    carry_flag(register_x >= value);
    zero_flag(value);
    negative_flag(value);
}

void CPU::dcp(enum addressing_mode mode){
    uint16_t address = get_operand_address(mode);
    uint8_t value = read_memory(address) - 1;
    write_memory(address, value);
    carry_flag(register_a >= value);
    zero_flag((uint8_t) (register_a - value));
    negative_flag((uint8_t) (register_a - value));
}

void CPU::dop(uint8_t opcode){
    // NOP * 2
}

void CPU::isc(enum addressing_mode mode){
    uint16_t address = get_operand_address(mode);
    uint8_t value = read_memory(address) + 1;
    write_memory(address, value);
    sbc(mode);
}

void CPU::kil(){
    // HALT
}

void CPU::lar(enum addressing_mode mode){
    register_a = register_x = stack_pointer = read_memory(get_operand_address(mode)) & stack_pointer;
    zero_flag(register_a);
    negative_flag(register_a);
}

void CPU::lax(enum addressing_mode mode){
    register_a = register_x = read_memory(get_operand_address(mode));
    zero_flag(register_a);
    negative_flag(register_a);
}

void CPU::rla(enum addressing_mode mode){
    uint16_t address = get_operand_address(mode);
    rol(mode);
    uint8_t value = read_memory(address);
    register_a = register_a & value;
    zero_flag(register_a);
    negative_flag(register_a);
}

void CPU::rra(enum addressing_mode mode){
    ror(mode);
    adc(mode);
}

void CPU::slo(enum addressing_mode mode){
    uint16_t address = get_operand_address(mode);
    asl(mode);
    uint8_t value = read_memory(address);
    register_a = register_a | value;
    zero_flag(register_a);
    negative_flag(register_a);
}

void CPU::sre(enum addressing_mode mode){
    uint16_t address = get_operand_address(mode);
    lsr(mode);
    uint8_t value = read_memory(address);
    register_a = register_a ^ value;
    zero_flag(register_a);
    negative_flag(register_a);
}

void CPU::sxa(enum addressing_mode mode){
    uint16_t address = get_operand_address(mode);
    write_memory(address, (register_x & (uint8_t) ((address >> 8)+1)));
}

void CPU::sya(enum addressing_mode mode){
    uint16_t address = get_operand_address(mode);
    write_memory(address, (register_y & (uint8_t) ((address >> 8)+1)));
}

void CPU::top(uint8_t opcode){
    // NOP * 3
}

void CPU::xaa(enum addressing_mode mode){
    register_a = register_x;
    zero_flag(register_a);
    negative_flag(register_a);
    uint16_t address = get_operand_address(mode);
    uint8_t value = read_memory(address);
    register_a = register_a & value;
}

void CPU::xas(enum addressing_mode mode){
    stack_pointer = register_a & register_x;
    uint16_t address = get_operand_address(mode);
    write_memory(address, (stack_pointer & (uint8_t) ((address >> 8)+1)));
}

void CPU::zero_flag(uint8_t reg) {

    status_register = (reg == 0) ? (CPU::status_register | ZERO_FLAG) : (CPU::status_register & ~ZERO_FLAG);
}

void CPU::negative_flag(uint8_t reg) {

    CPU::status_register = (reg & 0b10000000) ? (CPU::status_register | NEGATIVE_FLAG) : (CPU::status_register &
                                                                                          ~NEGATIVE_FLAG);

}

void CPU::carry_flag(bool flag) {
    CPU::status_register = flag ? (CPU::status_register | CARRY_FLAG) : (CPU::status_register & ~CARRY_FLAG);
}

void CPU::overflow_flag(bool flag) {
    CPU::status_register = flag ? (CPU::status_register | OVERFLOW_FLAG) : (CPU::status_register & ~OVERFLOW_FLAG);
}

void CPU::interrupt(Interrupt_type type) {

    if (type == Interrupt_type::NMI){
        stack_push_16(program_counter);
        uint8_t status = status_register;
        interrupt::interrupts.at(0).break_flag & 0b010000 ? status |= BREAK_FLAG : status &= ~BREAK_FLAG;
        interrupt::interrupts.at(0).break_flag & 0b100000 ? status |= BREAK_FLAG_2 : status &= ~BREAK_FLAG_2;
        stack_push(status);
        status_register |= INTERRUPT_FLAG;
        bus->tick(interrupt::interrupts.at(0).cycles);
        program_counter = read_memory_16(interrupt::interrupts.at(0).address);
    }
}

template<typename Callback>
void CPU::run_with_callback(Callback callback){
    bool show_frame = false;
    while(show_frame == false) {
        if (bus->Interrupt.has_value()) {

            interrupt(bus->Interrupt.value());
        }
        bus->Interrupt.reset();


        //callback(*this);

        //std::cout<<this->trace()<<std::endl;

        uint8_t opcode = read_memory(program_counter);
        //std::cout << Opcode::opcode_map.at(opcode).getMnemonic() << " " << std::hex << (int) opcode << " A: "<< std::dec << (int) register_a << " PC: " << program_counter << " SR: " << (int) status_register<< std::endl;

        program_counter++;
        uint16_t program_counter_status = program_counter;

        switch (opcode) {
            case 0x69:
            case 0x65:
            case 0x75:
            case 0x6D:
            case 0x7D:
            case 0x79:
            case 0x61:
            case 0x71:
                adc(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xe9:
            case 0xe5:
            case 0xf5:
            case 0xed:
            case 0xfd:
            case 0xf9:
            case 0xe1:
            case 0xf1:
                sbc(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xa9:
            case 0xa5:
            case 0xb5:
            case 0xad:
            case 0xbd:
            case 0xb9:
            case 0xa1:
            case 0xb1:
                lda(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x85:
            case 0x95:
            case 0x8D:
            case 0x9D:
            case 0x99:
            case 0x81:
            case 0x91:
                sta(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x29:
            case 0x25:
            case 0x35:
            case 0x2D:
            case 0x3D:
            case 0x39:
            case 0x21:
            case 0x31:
                logical_and(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x06:
            case 0x16:
            case 0x0e:
            case 0x1e:
                asl(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x0a:
                asl_accumulator();
                break;
            case 0x90:
                bcc();
                break;
            case 0xb0:
                bcs();
                break;
            case 0xf0:
                beq();
                break;
            case 0x24:
            case 0x2c:
                bit(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x30:
                bmi();
                break;
            case 0xd0:
                bne();
                break;
            case 0x10:
                bpl();
                break;
            case 0x50:
                bvc();
                break;
            case 0x70:
                bvs();
                break;
            case 0x18:
                clc();
                break;
            case 0xd8:
                cld();
                break;
            case 0x58:
                cli();
                break;
            case 0xb8:
                clv();
                break;
            case 0xc9:
            case 0xc5:
            case 0xd5:
            case 0xcd:
            case 0xdd:
            case 0xd9:
            case 0xc1:
            case 0xd1:
                cmp(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xe0:
            case 0xe4:
            case 0xec:
                cpx(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xc0:
            case 0xc4:
            case 0xcc:
                cpy(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xc6:
            case 0xd6:
            case 0xce:
            case 0xde:
                dec(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xca:
                dex();
                break;
            case 0x88:
                dey();
                break;
            case 0x49:
            case 0x45:
            case 0x55:
            case 0x4d:
            case 0x5d:
            case 0x59:
            case 0x41:
            case 0x51:
                eor(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xe6:
            case 0xf6:
            case 0xee:
            case 0xfe:
                inc(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xaa:
                tax();
                break;
            case 0xe8:
                inx();
                break;
            case 0xc8:
                iny();
                break;
            case 0x4c:
            case 0x6c:
                jmp(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x20:
                jsr();
                break;
            case 0xa2:
            case 0xa6:
            case 0xb6:
            case 0xae:
            case 0xbe:
                ldx(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xa0:
            case 0xa4:
            case 0xb4:
            case 0xac:
            case 0xbc:
                ldy(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x4a:
                lsr_accumulator();
                break;
            case 0x46:
            case 0x56:
            case 0x4e:
            case 0x5e:
                lsr(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xea:
                nop();
                break;
            case 0x09:
            case 0x05:
            case 0x15:
            case 0x0d:
            case 0x1d:
            case 0x19:
            case 0x01:
            case 0x11:
                ora(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x48:
                pha();
                break;
            case 0x08:
                php();
                break;
            case 0x68:
                pla();
                break;
            case 0x28:
                plp();
                break;
            case 0x2a:
                rol_accumulator();
                break;
            case 0x26:
            case 0x36:
            case 0x2e:
            case 0x3e:
                rol(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x6a:
                ror_accumulator();
                break;
            case 0x66:
            case 0x76:
            case 0x6e:
            case 0x7e:
                ror(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x40:
                rti();
                break;
            case 0x60:
                rts();
                break;
            case 0x38:
                sec();
                break;
            case 0xf8:
                sed();
                break;
            case 0x78:
                sei();
                break;
            case 0x86:
            case 0x96:
            case 0x8e:
                stx(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x84:
            case 0x94:
            case 0x8c:
                sty(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xa8:
                tay();
                break;
            case 0xba:
                tsx();
                break;
            case 0x8a:
                txa();
                break;
            case 0x9a:
                txs();
                break;
            case 0x98:
                tya();
                break;
            case 0x0b:
            case 0x2b:
                aac(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x87:
            case 0x97:
            case 0x83:
            case 0x8f:
                aax(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x6b:
                arr(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x4b:
                asr(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xab:
                atx(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x9f:
            case 0x93:
                axa(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xcb:
                axs(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xc7:
            case 0xd7:
            case 0xcf:
            case 0xdf:
            case 0xdb:
            case 0xc3:
            case 0xd3:
                dcp(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x04:
            case 0x14:
            case 0x34:
            case 0x44:
            case 0x54:
            case 0x64:
            case 0x74:
            case 0x80:
            case 0x82:
            case 0x89:
            case 0xc2:
            case 0xd4:
            case 0xe2:
            case 0xf4:
                dop(opcode);
                break;
            case 0xe7:
            case 0xf7:
            case 0xef:
            case 0xff:
            case 0xfb:
            case 0xe3:
            case 0xf3:
                isc(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x02:
            case 0x12:
            case 0x22:
            case 0x32:
            case 0x42:
            case 0x52:
            case 0x62:
            case 0x72:
            case 0x92:
            case 0xb2:
            case 0xd2:
            case 0xf2:
                kil();
                break;
            case 0xbb:
                lar(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xa7:
            case 0xb7:
            case 0xaf:
            case 0xbf:
            case 0xa3:
            case 0xb3:
                lax(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x1a:
            case 0x3a:
            case 0x5a:
            case 0x7a:
            case 0xda:
            case 0xfa:
                nop();
                break;
            case 0x27:
            case 0x37:
            case 0x2f:
            case 0x3f:
            case 0x3b:
            case 0x23:
            case 0x33:
                rla(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x67:
            case 0x77:
            case 0x6f:
            case 0x7f:
            case 0x7b:
            case 0x63:
            case 0x73:
                rra(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0xeb:
                sbc(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x07:
            case 0x17:
            case 0x0f:
            case 0x1f:
            case 0x1b:
            case 0x03:
            case 0x13:
                slo(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x47:
            case 0x57:
            case 0x4f:
            case 0x5f:
            case 0x5b:
            case 0x43:
            case 0x53:
                sre(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x9e:
                sxa(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x9c:
                sya(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x0c:
            case 0x1c:
            case 0x3c:
            case 0x5c:
            case 0x7c:
            case 0xdc:
            case 0xfc:
                top(opcode);
                break;
            case 0x8b:
                xaa(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x9b:
                xas(Opcode::opcode_map.at(opcode).getAddressingMode());
                break;
            case 0x00:
                return;
            default:
                return;
        }
        elapsed_cycles += Opcode::opcode_map.at(opcode).getCycles();
        if(bus->tick(Opcode::opcode_map.at(opcode).getCycles()) == 1){
            show_frame = true;
            end_time_frame(elapsed_cycles);
        }

        if (program_counter_status == program_counter) {
            program_counter += Opcode::opcode_map.at(opcode).getBytes() - 1;
        }

    }


}
void CPU::run() {

    run_with_callback([](CPU &) {});
}

std::string CPU::trace() {

    uint8_t code = read_memory(get_program_counter());
    Opcode opcode = Opcode::opcode_map[code];

    uint16_t pc = program_counter;
    std::vector<uint8_t> hex_dump;
    hex_dump.push_back(code);

    uint16_t mem_address = 0;
    uint8_t mem_value = 0;
    switch (opcode.getAddressingMode()) {
        case addressing_mode::IMMEDIATE:
        case addressing_mode::NONE_ADDRESSING:
            break;
        default:
            program_counter++;
            mem_address = get_operand_address(opcode.getAddressingMode());
            mem_value = read_memory(mem_address);
            program_counter--;
    }
    std::ostringstream ss;
    switch (opcode.getBytes()) {
        case 1:
            if (opcode.getOpcode() == 0x0a || opcode.getOpcode() ==0x4a || opcode.getOpcode() == 0x2a || opcode.getOpcode() == 0x6a){
                ss << "A ";
            }
            break;
        case 2: {
            uint8_t address = read_memory(pc + 1);
            hex_dump.push_back(address);

            switch (opcode.getAddressingMode()) {
                case addressing_mode::IMMEDIATE:
                    ss << "#$"  << std::setw(2) << std::setfill('0')<<std::hex << (int) address;
                    break;
                case addressing_mode::ZERO_PAGE:
                    ss << "$" << std::setw(2) << std::setfill('0')<<std::hex << (int) mem_address << " = " << std::setw(2) << std::setfill('0')<<std::hex<< (int)  mem_value;
                    break;
                case addressing_mode::ZERO_PAGE_X:
                    ss << "$" << std::setw(2) << std::setfill('0')<<std::hex<< (int) address << ",X @ "  <<std::setw(2) << std::setfill('0')<< std::hex << (int) mem_address << " = "  <<std::setw(2) << std::setfill('0')<< std::hex << (int) mem_value;
                    break;
                case addressing_mode::ZERO_PAGE_Y:
                    ss << "$" << std::setw(2) << std::setfill('0')<<std::hex << (int) address << ",Y @ "  <<std::setw(2) << std::setfill('0')<<std::hex << (int) mem_address << " = "  <<std::setw(2) << std::setfill('0')<<std::hex <<(int) mem_value;
                    break;
                case addressing_mode::INDIRECT_X:
                    ss << "($" <<std::setw(2) << std::setfill('0')<<std::hex << (int) address << ",X) @ " <<std::setw(2) << std::setfill('0')<< std::hex << (int) ((uint8_t) (address  + register_x)) << " = "  <<std::setw(4) <<std::right << std::setfill('0')<<std::hex <<(int) mem_address << " = "  <<std::setw(2) << std::setfill('0')<< std::hex << (int) mem_value;
                    break;
                case addressing_mode::INDIRECT_Y:
                    ss << "($" << std::setw(2) << std::setfill('0')<<std::hex << (int) address << "),Y = " <<std::setw(4) << std::setfill('0')<<std::hex  << (int)  ( (uint16_t) (mem_address - register_y))<< " @ " <<std::setw(4) << std::setfill('0')<< std::hex <<(int) mem_address << " = " <<std::setw(2) << std::setfill('0')<< std::hex << (int) mem_value;
                    break;
                case addressing_mode::RELATIVE:
                    ss << "$" << std::setw(4) << std::setfill('0')<< std::hex << (pc+2) + int8_t (address);
                    break;
                default:
                    throw std::runtime_error("Not implemented addressing mode");
            }
            break;
        }
            case 3: {
                uint8_t address_low = read_memory(pc + 1);
                uint8_t address_high = read_memory(pc + 2);
                hex_dump.push_back(address_low);
                hex_dump.push_back(address_high);

                uint16_t address = (address_high << 8) | address_low;
                switch (opcode.getAddressingMode()) {

                    case addressing_mode::ABSOLUTE:
                        if(opcode.getOpcode() == 0x4c || opcode.getOpcode() == 0x20){
                            ss << "$" << std::setw(4) << std::setfill('0')<< std::hex << (int) address;
                        }else {
                            ss << "$" << std::setw(4) << std::setfill('0')<< std::hex<< (int) mem_address << " = " <<  std::setw(2) << std::hex << std::setfill('0') << (int) mem_value;
                        }
                        break;
                    case addressing_mode::ABSOLUTE_X:
                        ss << "$" <<std::setw(4) << std::setfill('0')<< std::hex << (int) address << ",X @ " <<std::setw(4) << std::setfill('0')<< std::hex << (int) mem_address << " = " <<std::setw(2) << std::setfill('0')<< std::hex << (int) mem_value;
                        break;
                    case addressing_mode::ABSOLUTE_Y:
                        ss << "$"  <<std::setw(4) << std::setfill('0')<< std::hex <<  (int) address << ",Y @ "  <<std::setw(4) << std::setfill('0')<< std::hex << (int) mem_address << " = "  <<std::setw(2) << std::setfill('0')<< std::hex << (int) mem_value;
                        break;
                    case addressing_mode::INDIRECT: {
                        uint16_t jmp_address = 0;
                        if((address & 0x00FF) == 0x00FF) {
                            jmp_address = (read_memory(address & 0xFF00) << 8) | read_memory(address);
                        }else {
                            jmp_address = read_memory_16(address);
                        }
                        ss << "($"  <<std::setw(4) << std::setfill('0')<<std::hex <<(int) address << ") = "  <<std::setw(4) << std::setfill('0')<< std::hex << (int) jmp_address;
                        break;
                    }
                    default:
                        throw std::runtime_error("Not implemented addressing mode");
                }
                break;
            }
            default:
                ss << "Not implemented";



            }
    std::ostringstream hex_str;
    for (const auto &z : hex_dump) {
        hex_str << std::setw(2) << std::setfill('0') << std::hex << (int) z;
        hex_str << " ";
    }


    std::ostringstream asm_oss;
    asm_oss << std::setw(4) << std::setfill('0') << std::hex << pc << "  "  << std::setw(9)  << std::setfill(' ') <<std::left <<  hex_str.str() << " " << opcode.getMnemonic() << " " << std::setw(27)  <<std::left << ss.str();
    asm_oss << " A:" <<std::right << std::setw(2) << std::setfill('0') << std::hex << (int) get_register_a() << " X:" << std::right << std::setw(2) << std::setfill('0') << std::hex << (int) get_register_x() << " Y:" << std::right << std::setw(2) << std::setfill('0') << std::hex << (int) get_register_y();
    asm_oss << " P:" <<std::right << std::setw(2) << std::setfill('0') << std::hex << (int) get_status_register() << " SP:" <<std::right << std::setw(2) << std::setfill('0') << std::hex << (int) get_stack_pointer();
    std::string asm_str = asm_oss.str();

    std::transform(asm_str.begin(), asm_str.end(), asm_str.begin(), ::toupper);
    return asm_str;
    }

void CPU::trigger_nmi() {
    bus->Interrupt.emplace(Interrupt_type::NMI);
}

void CPU::trigger_irq() {
    if((get_status_register() & 0b00000100) == 0) {
        bus->Interrupt.emplace(Interrupt_type::IRQ);
    }
}

void CPU::init_apu(){
    blargg_err_t err = blip_buffer.sample_rate(44100);
    if (err){
        // TODO: error handling

    }
    blip_buffer.clock_rate(1789773);
    sound_queue = new Sound_Queue;
    sound_queue->init(44100);
    apu.output(&blip_buffer);
    apu.dmc_reader(&CPU::dmc_read, this);

}

void CPU::end_time_frame(int length) {
    apu.end_frame(length);
    blip_buffer.end_frame(length);
    elapsed_cycles =0;

    // out_size = 4096;
    if(blip_buffer.samples_avail() >= 4096){
        long count = blip_buffer.read_samples(blip_out_buffer, 4096);
        sound_queue->write(blip_out_buffer, count);
    }

}

int CPU::dmc_read(void* data, cpu_addr_t address){
    return ((CPU*)data)->read_memory(address);
}

/*
TEST(lda_load_data, lda_load_data) {
    CPU cpu;
    std::vector<uint8_t> instructions = {0xA9, 0x05, 0x00};
    cpu.fetch_and_exectute(instructions);
    ASSERT_EQ(cpu.get_register_a(), 0x05);
    ASSERT_EQ(cpu.get_status_register() & 0b00000010, 0b00000000);
    ASSERT_EQ(cpu.get_status_register() & 0b10000000, 0b00000000);

}

TEST(lda_zero_flag,lda_zero_flag){
    CPU cpu;
    std::vector<uint8_t> instructions = {0xA9, 0x00, 0x00};
    cpu.fetch_and_exectute(instructions);
    ASSERT_EQ(cpu.get_status_register(), 0b00000010);
}

TEST(lda_negative_flag,lda_negative_flag){
    CPU cpu;
    std::vector<uint8_t> instructions = {0xA9, 0xFF, 0x00};
    cpu.fetch_and_exectute(instructions);
    ASSERT_EQ(cpu.get_status_register(), 0b10000000);
}

TEST(tax_move_register_a_to_register_x,tax_move_register_a_to_register_x){
    CPU cpu;
    std::vector<uint8_t> instructions = {0xA9, 0x05, 0xaa, 0x00};
    cpu.fetch_and_exectute(instructions);
    ASSERT_EQ(cpu.get_register_x(), 0x05);
    ASSERT_EQ(cpu.get_status_register(), 0b00000000);
}

TEST(adc_add_with_carry,adc_add_with_carry){
    CPU cpu;
    std::vector<uint8_t> instructions = {0xA9, 0x05, 0x69, 0x05,0x00};
    cpu.fetch_and_exectute(instructions);
    ASSERT_EQ(cpu.get_register_a(), 0x0A);
    ASSERT_EQ(cpu.get_status_register(), 0b00000000);
}
TEST(adc_add_with_carry_overflow,adc_add_with_carry_overflow){
    CPU cpu;
    std::vector<uint8_t> instructions = {0xA9, 0x7F, 0x69, 0x01, 0x00};
    cpu.fetch_and_exectute(instructions);
    ASSERT_EQ(cpu.get_register_a(), 0x80);
    ASSERT_EQ(cpu.get_status_register(), 0b11000000);
}

TEST(adc_add_with_carry_carry,adc_add_with_carry_carry){
    CPU cpu;
    std::vector<uint8_t> instructions = {0xA9, 0xFF, 0x69, 0x01, 0x00};
    cpu.fetch_and_exectute(instructions);
    ASSERT_EQ(cpu.get_register_a(), 0x00);
    ASSERT_EQ(cpu.get_status_register(), 0b00000011);
}
TEST(adc_add_zero_to_zero,adc_add_zero_to_zero){
    CPU cpu;
    std::vector<uint8_t> instructions = {0xA9, 0x00, 0x69, 0x00, 0x00};
    cpu.fetch_and_exectute(instructions);
    ASSERT_EQ(cpu.get_register_a(), 0x00);
    ASSERT_EQ(cpu.get_status_register(), 0b00000010);
}

TEST(adc_add_absolute, adc_add_absolute){
    CPU cpu;
    std::vector<uint8_t> instructions = {0xA9, 0x05, 0x6D, 0x01, 0x00, 0x00};
    cpu.fetch_and_exectute(instructions);
    ASSERT_EQ(cpu.get_register_a(), 0x0A);
    ASSERT_EQ(cpu.get_status_register(), 0b00000000);
}

TEST(sbc_subtract_with_carry, sbc_subtract_with_carry){
    CPU cpu;
    std::vector<uint8_t> instructions = {0xA9, 0x05, 0xE9, 0x05, 0x00};
    cpu.fetch_and_exectute(instructions);
    ASSERT_EQ(cpu.get_register_a(), 0x00);
    ASSERT_EQ(cpu.get_status_register(), 0b00000010);
}


TEST(sbc_subtract_absolute, sbc_subtract_absolute) {
    CPU cpu;
    std::vector<uint8_t> instructions = {0xA9, 0x05, 0xED, 0x01, 0x00, 0x00};
    cpu.fetch_and_exectute(instructions);
    ASSERT_EQ(cpu.get_register_a(), 0x00);
    ASSERT_EQ(cpu.get_status_register(), 0b00000010);
}

TEST(logical_and, logical_and) {
    CPU cpu;
    std::vector<uint8_t> instructions = {0xA9, 0x05, 0x29, 0x05, 0x00};
    cpu.fetch_and_exectute(instructions);
    ASSERT_EQ(cpu.get_register_a(), 0x05);
    ASSERT_EQ(cpu.get_status_register(), 0b00000000);
}

TEST(asl_shift_left, asl_shift_left) {
    CPU cpu;
    std::vector<uint8_t> instructions = {0x06, 0x00, 0x00};
    cpu.fetch_and_exectute(instructions);
    ASSERT_EQ(cpu.read_memory(0x00), 0x0c);
    ASSERT_EQ(cpu.get_status_register(), 0b00000000);
}
TEST(asl_carry, asl_carry) {
    CPU cpu;
    std::vector<uint8_t> instructions = {0x0e, 0x04, 0x00,0x00, 0xfe};
    cpu.fetch_and_exectute(instructions);
    ASSERT_EQ(cpu.read_memory(0x04), 0xFC);
    ASSERT_EQ(cpu.get_status_register(), 0x81);
}

TEST(bit_test_bit, bit_test_bit) {
    CPU cpu;
    std::vector<uint8_t> instructions = {0xa9, 0x05, 0x2c, 0x04, 0x00,0x00};
    cpu.fetch_and_exectute(instructions);
    ASSERT_EQ(cpu.get_status_register(), ZERO_FLAG);
}
 */
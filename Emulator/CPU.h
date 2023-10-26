#ifndef NES_EMULATOR_CPU_H
#define NES_EMULATOR_CPU_H
#include <vector>
#include <array>
#include <map>
#include "addressing_mode.h"
#include "Opcode.h"
#include <iostream>
#include <bitset>
#include "Bus.h"
#include <sstream>
#include <fstream>
#include "nes_apu/Blip_Buffer.h"
#include "Simple_Apu.h"
#include "Sound_Queue.h"

namespace interrupt{


    struct Interrupt{
        Interrupt_type type;
        uint16_t address;
        uint8_t break_flag;
        uint8_t cycles;
    };

    const std::vector<Interrupt> interrupts = {
            {Interrupt_type::NMI, 0xFFFA, 0x20, 2},
    };
}

class CPU {

public:
    CPU();
    ~CPU();
    CPU(Bus *_bus);


    void run();
    template<typename Callback>
    void run_with_callback(Callback callback);




    // accessors
    uint8_t get_register_a() const;
    uint8_t get_register_x() const;
    uint8_t get_register_y() const;
    uint8_t get_stack_pointer() const;
    uint8_t get_status_register() const;
    uint16_t get_program_counter() const;
    void set_program_counter(uint16_t program_counter);



    // Instructions

    void sta(enum addressing_mode mode);
    void adc(enum addressing_mode mode);
    void logical_and(enum addressing_mode mode);
    void asl(enum addressing_mode mode);
    void asl_accumulator();
    void bcc();
    void bcs();
    void beq();
    void bit(enum addressing_mode mode);
    void bmi();
    void bne();
    void bpl();
    void bvc();
    void bvs();
    void clc();
    void cld();
    void cli();
    void clv();
    void cmp(enum addressing_mode mode);
    void cpx(enum addressing_mode mode);
    void cpy(enum addressing_mode mode);
    void dec(enum addressing_mode mode);
    void dex();
    void dey();
    void eor(enum addressing_mode mode);
    void inc(enum addressing_mode mode);
    void inx();
    void iny();
    void jmp(enum addressing_mode mode);
    void jsr();
    void lda(enum addressing_mode mode);
    void ldx(enum addressing_mode mode);
    void ldy(enum addressing_mode mode);
    void lsr(enum addressing_mode mode);
    void lsr_accumulator();
    void nop();
    void ora(enum addressing_mode mode);
    void pha();
    void php();
    void pla();
    void plp();
    void rol(enum addressing_mode mode);
    void rol_accumulator();
    void ror(enum addressing_mode mode);
    void ror_accumulator();
    void rti();
    void rts();
    void sbc(enum addressing_mode mode);
    void sec();
    void sed();
    void sei();
    void stx(enum addressing_mode mode);
    void sty(enum addressing_mode mode);
    void tax();
    void tay();
    void tsx();
    void txa();
    void txs();
    void tya();
    void aac(enum addressing_mode mode);
    void aax(enum addressing_mode mode);
    void arr(enum addressing_mode mode);
    void asr(enum addressing_mode mode);
    void atx(enum addressing_mode mode);
    void axa(enum addressing_mode mode);
    void axs(enum addressing_mode mode);
    void dcp(enum addressing_mode mode);
    void dop(uint8_t opcode);
    void isc(enum addressing_mode mode);
    void kil();
    void lar(enum addressing_mode mode);
    void lax(enum addressing_mode mode);
    void nop(uint8_t opcode);
    void rla(enum addressing_mode mode);
    void rra(enum addressing_mode mode);
    void slo(enum addressing_mode mode);
    void sre(enum addressing_mode mode);
    void sxa(enum addressing_mode mode);
    void sya(enum addressing_mode mode);
    void top(uint8_t opcode);
    void xaa(enum addressing_mode mode);
    void xas(enum addressing_mode mode);


    // Flags
    void zero_flag(uint8_t reg);
    void negative_flag(uint8_t reg);
    void carry_flag(bool flag);
    void overflow_flag(bool flag);

    // Memory
    uint8_t read_memory(uint16_t address);
    void write_memory(uint16_t address, uint8_t value);
    void fetch_and_exectute(std::vector<uint8_t> program);
    void load(std::vector<uint8_t> program);
    void write_memory_16(uint16_t address, uint16_t value);
    uint16_t read_memory_16(uint16_t address);
    void reset();

    // Stack Operations
    void stack_push(uint8_t value);
    uint8_t stack_pop();
    void stack_push_16(uint16_t value);
    uint16_t stack_pop_16();


    uint16_t get_operand_address(addressing_mode mode);

    bool page_crossed(addressing_mode addressing_mode);
    bool page_crossed(uint16_t address1, uint16_t address2);

    void interrupt(Interrupt_type type);
    void trigger_nmi();
    void trigger_irq();


    std::string trace();
    static int dmc_read(void* data, cpu_addr_t address);
    void end_time_frame(int length);
    void init_apu();


private:
    uint8_t register_x;
    uint8_t register_a;
    uint8_t register_y;
    uint8_t status_register;
    uint16_t program_counter;
    uint8_t stack_pointer;
    std::array<uint8_t , 65535> memory{};

    Bus *bus;
    Blip_Buffer blip_buffer;
    Nes_Apu apu;
    int elapsed_cycles;
    blip_sample_t blip_out_buffer[4096];
    Sound_Queue *sound_queue;




};


#endif

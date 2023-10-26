

#ifndef NES_EMULATOR_PPU_H
#define NES_EMULATOR_PPU_H


#include <cstdint>
#include <array>
#include <vector>
#include "Mapper.h"
#include <optional>


struct PALETTE_COLOR{
    PALETTE_COLOR(int i, int i1, int i2);

    uint8_t r;
    uint8_t g;
    uint8_t b;
};

const std::array<PALETTE_COLOR, 64> SYSTEM_PALETTE = {
        {
                {0x80, 0x80, 0x80}, {0x00, 0x3D, 0xA6}, {0x00, 0x12, 0xB0}, {0x44, 0x00, 0x96}, {0xA1, 0x00, 0x5E},
                {0xC7, 0x00, 0x28}, {0xBA, 0x06, 0x00}, {0x8C, 0x17, 0x00}, {0x5C, 0x2F, 0x00}, {0x10, 0x45, 0x00},
                {0x05, 0x4A, 0x00}, {0x00, 0x47, 0x2E}, {0x00, 0x41, 0x66}, {0x00, 0x00, 0x00}, {0x05, 0x05, 0x05},
                {0x05, 0x05, 0x05}, {0xC7, 0xC7, 0xC7}, {0x00, 0x77, 0xFF}, {0x21, 0x55, 0xFF}, {0x82, 0x37, 0xFA},
                {0xEB, 0x2F, 0xB5}, {0xFF, 0x29, 0x50}, {0xFF, 0x22, 0x00}, {0xD6, 0x32, 0x00}, {0xC4, 0x62, 0x00},
                {0x35, 0x80, 0x00}, {0x05, 0x8F, 0x00}, {0x00, 0x8A, 0x55}, {0x00, 0x99, 0xCC}, {0x21, 0x21, 0x21},
                {0x09, 0x09, 0x09}, {0x09, 0x09, 0x09}, {0xFF, 0xFF, 0xFF}, {0x0F, 0xD7, 0xFF}, {0x69, 0xA2, 0xFF},
                {0xD4, 0x80, 0xFF}, {0xFF, 0x45, 0xF3}, {0xFF, 0x61, 0x8B}, {0xFF, 0x88, 0x33}, {0xFF, 0x9C, 0x12},
                {0xFA, 0xBC, 0x20}, {0x9F, 0xE3, 0x0E}, {0x2B, 0xF0, 0x35}, {0x0C, 0xF0, 0xA4}, {0x05, 0xFB, 0xFF},
                {0x5E, 0x5E, 0x5E}, {0x0D, 0x0D, 0x0D}, {0x0D, 0x0D, 0x0D}, {0xFF, 0xFF, 0xFF}, {0xA6, 0xFC, 0xFF},
                {0xB3, 0xEC, 0xFF}, {0xDA, 0xAB, 0xEB}, {0xFF, 0xA8, 0xF9}, {0xFF, 0xAB, 0xB3}, {0xFF, 0xD2, 0xB0},
                {0xFF, 0xEF, 0xA6}, {0xFF, 0xF7, 0x9C}, {0xD7, 0xE8, 0x95}, {0xA6, 0xED, 0xAF}, {0xA2, 0xF2, 0xDA},
                {0x99, 0xFF, 0xFC}, {0xDD, 0xDD, 0xDD}, {0x11, 0x11, 0x11}, {0x11, 0x11, 0x11}
        }
};

class ControlRegister{
#define NAMETABLE1             0x01
#define NAMETABLE2             0x02
#define VRAM_ADD_INCREMENT     0x04
#define SPRITE_PATTERN_ADDR    0x08
#define BACKROUND_PATTERN_ADDR 0x10
#define SPRITE_SIZE            0x20
#define MASTER_SLAVE_SELECT    0x40
#define GENERATE_NMI           0x80



public:
    ControlRegister();
    ~ControlRegister() = default;
    uint16_t nametable_address();
    uint8_t vram_address_increment();
    uint16_t sprite_pattern_address();
    uint16_t background_pattern_address();
    uint8_t sprite_size();
    uint8_t master_slave_select();
    uint8_t generate_nmi();
    uint8_t get_control_register();
    void set_control_register(uint8_t data);
private:
    uint8_t control_register;
};
class MaskRegister {
#define GRAYSCALE             0x01
#define BACKGROUND_LEFT       0x02
#define SPRITES_LEFT          0x04
#define BACKGROUND_ENABLE     0x08
#define SPRITES_ENABLE        0x10
#define EMPHASIZE_RED         0x20
#define EMPHASIZE_GREEN       0x40
#define EMPHASIZE_BLUE        0x80

public:
    MaskRegister();
    ~MaskRegister() = default;
    uint8_t grayscale();
    uint8_t background_left();
    uint8_t sprites_left();
    uint8_t background_enable();
    uint8_t sprites_enable();
    uint8_t emphasize_red();
    uint8_t emphasize_green();
    uint8_t emphasize_blue();
    uint8_t get_mask_register();
    void set_mask_register(uint8_t data);
private:
    uint8_t mask_register;


};

class StatusRegister {
#define SPRITE_OVERFLOW        0x20
#define SPRITE_ZERO_HIT        0x40
#define VERTICAL_BLANK         0x80

public:
    StatusRegister();
    ~StatusRegister() = default;
    uint8_t sprite_overflow();
    uint8_t sprite_zero_hit();
    uint8_t vertical_blank();
    uint8_t get_status_register();
    void set_status_register(uint8_t data);
    void clear_sprite_zero_hit();
    void clear_vertical_blank();
    void clear_sprite_overflow();
private:
    uint8_t status_register;
};
class PPU_Data{
public:
    explicit PPU_Data(Mapper *mapper);
    PPU_Data();
    ~PPU_Data() = default;
    uint8_t read(uint16_t address);
    int name_table_mirroring(uint16_t address);
    void write(uint16_t address, uint8_t data);
private:
    std::array<uint8_t, 0x1000> nametables;
    std::array<uint8_t, 0x20> palette_tables;
    Mapper *mapper;
};


struct ResultFrame{
    bool nmi;
    bool frame_complete;
};

class PPU {
public:
    PPU() = default;
    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t data);
    PPU(Mapper *mapper);
    ~PPU() = default;
    std::vector<uint8_t> get_frame();
    bool rendering_enabled();
    void increment_horizontal_scroll();
    void increment_vertical_scroll();
    std::optional<uint8_t> get_background_pixel();
    std::optional<std::pair<int,uint8_t>> get_sprite_pixel();
    uint32_t fetch_sprite_pattern(uint16_t i,int16_t  row);
    void evaluate_sprites();
    void render_pixel();
    uint8_t get_nametable_byte();
    uint8_t get_attribute_table_byte();
    void store_tile_data();
    void tick(ResultFrame &result);
    ResultFrame step();
    ControlRegister control_register;
    MaskRegister mask_register;
    StatusRegister statusRegister;

private:


    uint8_t oam_address;
    std::array<uint8_t, 0x100> oam_data;

    uint16_t ppu_address;
    PPU_Data ppu_data_buffer;

    uint16_t dot;
    uint16_t scanline;

    uint8_t name_table_byte;
    uint8_t attribute_table_byte;
    uint8_t low_tile_byte;
    uint8_t high_tile_byte;
    uint32_t tile_data_high;
    uint32_t tile_data_low;

    int sprite_count;
    std::array<uint32_t , 8> sprite_patterns;
    std::array<uint32_t, 8> sprite_positions;
    std::array<uint32_t, 8> sprite_priorities;
    std::array<uint32_t, 8> sprite_indexes;

    bool odd_frame;

    bool nmi_occurred;
    bool nmi_output;
    bool nmi_previous;
    int nmi_delay;

    uint16_t t;
    uint8_t x;
    bool w;

    uint8_t ppu_buffer;
    uint8_t last_value;

    std::vector<uint8_t> frame;

    void is_nmi();

    uint8_t get_background_byte(bool low);
};



#endif

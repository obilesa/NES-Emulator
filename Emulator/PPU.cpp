#include <iostream>
#include "PPU.h"



ControlRegister::ControlRegister(){
    control_register = 0x00;
}

uint16_t ControlRegister::nametable_address() {
    switch (control_register & 0b11) {
        case 0:
            return 0x2000;
        case 1:
            return 0x2400;
        case 2:
            return 0x2800;
        case 3:
            return 0x2C00;
    }
    return 0;
}

uint8_t ControlRegister::vram_address_increment() {
    return (control_register & VRAM_ADD_INCREMENT) ? 32 : 1;
}

uint16_t ControlRegister::sprite_pattern_address() {
    return (control_register & SPRITE_PATTERN_ADDR) ? 0x1000 : 0x0000;
}

uint16_t ControlRegister::background_pattern_address() {
    return (control_register & BACKROUND_PATTERN_ADDR) ? 0x1000 : 0x0000;
}

uint8_t ControlRegister::sprite_size() {
    return (control_register & SPRITE_SIZE) ? 16 : 8;
}

uint8_t ControlRegister::master_slave_select() {
    return (control_register & MASTER_SLAVE_SELECT) ? 1 : 0;
}

uint8_t ControlRegister::generate_nmi() {
    return (control_register & GENERATE_NMI) ? 1 : 0;
}

uint8_t ControlRegister::get_control_register() {
    return control_register;
}

void ControlRegister::set_control_register(uint8_t data) {
    control_register = data;
}

MaskRegister::MaskRegister() {
    mask_register = 0x00;
}

uint8_t MaskRegister::grayscale() {
    return (mask_register & GRAYSCALE) ? 1 : 0;
}

uint8_t MaskRegister::background_left() {
    return (mask_register & BACKGROUND_LEFT) ? 1 : 0;
}

uint8_t MaskRegister::sprites_left() {
    return (mask_register & SPRITES_LEFT) ? 1 : 0;
}

uint8_t MaskRegister::background_enable() {
    return (mask_register & BACKGROUND_ENABLE) ? 1 : 0;
}

uint8_t MaskRegister::sprites_enable() {
    return (mask_register & SPRITES_ENABLE) ? 1 : 0;
}

uint8_t MaskRegister::emphasize_red() {
    return (mask_register & EMPHASIZE_RED) ? 1 : 0;
}

uint8_t MaskRegister::emphasize_green() {
    return (mask_register & EMPHASIZE_GREEN) ? 1 : 0;
}

uint8_t MaskRegister::emphasize_blue() {
    return (mask_register & EMPHASIZE_BLUE) ? 1 : 0;
}

uint8_t MaskRegister::get_mask_register() {
    return mask_register;
}

void MaskRegister::set_mask_register(uint8_t data) {
    mask_register = data;
}

StatusRegister::StatusRegister() {
    status_register = 0x00;
}

uint8_t StatusRegister::sprite_overflow() {
    return (status_register & SPRITE_OVERFLOW) ? 1 : 0;
}

uint8_t StatusRegister::sprite_zero_hit() {
    return (status_register & SPRITE_ZERO_HIT) ? 1 : 0;
}

uint8_t StatusRegister::vertical_blank() {
    return (status_register & VERTICAL_BLANK) ? 1 : 0;
}

uint8_t StatusRegister::get_status_register() {
    return status_register;
}

void StatusRegister::set_status_register(uint8_t data) {
    status_register = data;
}

void StatusRegister::clear_sprite_zero_hit() {
    status_register &= ~SPRITE_ZERO_HIT;

}

void StatusRegister::clear_vertical_blank() {
    status_register &= ~VERTICAL_BLANK;
}

void StatusRegister::clear_sprite_overflow() {
    status_register &= ~SPRITE_OVERFLOW;

}

uint8_t PPU::read(uint16_t address) {
    address = address % 8 + 0x2000;
    switch (address) {
        case 0x2000: {
            return control_register.get_control_register();
        }
        case 0x2001: {
            return  mask_register.get_mask_register();
        }
        case 0x2002: {
            uint8_t s_register = statusRegister.get_status_register();

            s_register &= ~0x1f;
            s_register |= last_value & 0x1f;

            if (nmi_occurred) {
                s_register |= 1 << 7;
            }
            nmi_occurred = false;
            is_nmi();
            w = false;

            return s_register;
        }
        case 0x2003:
            throw std::invalid_argument("PPU::read: OAMADDR is write-only");
        case 0x2004:
            return oam_data[oam_address % 0x100];
        case 0x2005:
            throw std::invalid_argument("PPU::read: PPUSCROLL is write-only");
        case 0x2006:
            throw std::invalid_argument("PPU::read: PPUADDR is write-only");
        case 0x2007: {
            uint8_t result;
            if (ppu_address % 0x4000 <= 0x3eff) {
                result = ppu_buffer;
                ppu_buffer = ppu_data_buffer.read(ppu_address);
            } else {

                ppu_buffer = ppu_data_buffer.read(ppu_address - 0x1000);
                result = ppu_data_buffer.read(ppu_address);
            }

            ppu_address = ppu_address + control_register.vram_address_increment();


            return result;
        }
        default:
            throw std::invalid_argument("bad PPU address " + std::to_string(address));
    }

}

void PPU::is_nmi() {
    auto nmi = nmi_output && nmi_occurred;
    if(nmi && !nmi_previous) {
        nmi_delay = 15;
    }
    nmi_previous = nmi;

}

void PPU::write(uint16_t address, uint8_t data) {
    last_value = data;

    address = (address % 8) + 0x2000;
    switch (address) {
        case 0x2000:
            control_register.set_control_register(data);


            t = (t & 0xf3ff) | (((uint16_t)data & 0x03) << 10);

            nmi_output = ((data >> 7) & 1) == 1;
            is_nmi();
            break;
        case 0x2001:
            mask_register.set_mask_register(data);
            break;
        case 0x2002:
            break;
        case 0x2003:
            oam_address = data;
            break;
        case 0x2004:
            address = address % 0x100;
            if(address % 4 == 2){
                auto v = data & 0xe3;
                oam_data[oam_address] = v;
            }else{
                oam_data[oam_address] = data;
            }
            oam_address++;
            break;
        case 0x2005:
            if (w) {
                t = (t & 0x8fff) | (((uint16_t)data & 0x07) << 12);
                t = (t & 0xfc1f) | (((uint16_t)data & 0xf8) << 2);
                w = false;
            } else {
                t = (t & 0xffe0) | ((uint16_t)data >> 3);
                x = data & 0x07;
                w = true;
            }
            break;
        case 0x2006:
            if (w) {
                t = (t & 0xff00) | (uint16_t)data;
                ppu_address = t;
                w = false;
            } else {
                t = (t & 0x80ff) | (((uint16_t)data & 0x3f) << 8);
                w = true;
            }
            break;
        case 0x2007:
            ppu_data_buffer.write(ppu_address, data);
            ppu_address = ppu_address + control_register.vram_address_increment();
            break;
        default:
            throw std::runtime_error("bad PPU address");
    }

}

PPU::PPU(Mapper *mapper) {
    control_register = ControlRegister();
    mask_register = MaskRegister();
    statusRegister = StatusRegister();
    oam_data.fill(0);
    oam_address = 0;
    ppu_address = 0;
    ppu_data_buffer = PPU_Data(mapper);

    dot = 0;
    scanline = 0;

    name_table_byte = 0;
    attribute_table_byte = 0;
    low_tile_byte = 0;
    high_tile_byte = 0;
    tile_data_high = 0;
    tile_data_low = 0;

    sprite_count = 0;
    sprite_patterns.fill(0);
    sprite_positions.fill(0);
    sprite_priorities.fill(0);
    sprite_indexes.fill(0);

    odd_frame = false;

    nmi_occurred = false;
    nmi_output = false;
    nmi_previous = false;
    nmi_delay = 0;

    t = 0;
    x = 0;
    w = false;

    ppu_buffer = 0;

    last_value = 0;

    frame.resize(240*256*3);

}

std::vector<uint8_t> PPU::get_frame() {
    return frame;
}

bool PPU::rendering_enabled() {
    return mask_register.background_enable() || mask_register.sprites_enable();
}

void PPU::increment_horizontal_scroll() {
    ppu_address = ((ppu_address & 0x001f) == 31) ? (ppu_address & ~0x001f) ^ 0x0400 : ppu_address + 1;
}

void PPU::increment_vertical_scroll() {
    //If the ppu_address is not in the bottom row, increment by 0x1000
    if ((ppu_address & 0x7000) != 0x7000) {
        ppu_address += 0x1000;
    } else {
        //Reset bottom row
        ppu_address &= ~0x7000;

        //Get the y coordinate
        uint16_t y = (ppu_address & 0x03e0) >> 5;

        switch (y) {
            case 29:
                y = 0;
                ppu_address ^= 0x0800;
                break;
            case 31:
                y = 0;
                break;
            default:
                y += 1;
                break;
        }

        //Set the y coordinate
        ppu_address = (ppu_address & ~0x03e0) | (y << 5);
    }
}



std::optional<uint8_t> PPU::get_background_pixel() {
    if (!mask_register.background_enable()) {
        return std::nullopt;
    }

    uint32_t t_data = tile_data_high;
    t_data = t_data >> ((7 - x) * 4);
    auto color = static_cast<uint8_t>(t_data & 0x0f);

    return color;
}




std::optional<std::pair<int,uint8_t>> PPU::get_sprite_pixel() {
    if (!mask_register.sprites_enable()) {
        return std::nullopt;
    }

    for (size_t i = 0; i < sprite_count; i++) {
        auto offset = (short) (((short)dot - 1) - sprite_positions[i]);

        if (offset < 0 || offset > 7) {
            continue;
        }


        offset = 7 - offset;

        uint8_t color = ((sprite_patterns[i] >> (offset * 4)) & 0x0f);


        if (color % 4 == 0) {
            continue;
        }
        return std::make_pair(i, color);
    }

    return std::nullopt;
}

uint32_t PPU::fetch_sprite_pattern(uint16_t  i, int16_t row) {
    uint16_t tile = oam_data[(i * 4 + 1) % 0x100];
    uint8_t attributes = oam_data[(i * 4 + 2) % 0x100];
    uint16_t address;
    if (control_register.sprite_size()== 8) {
        if (attributes & 0x80) {

            row = 7 - row;
        }
        address = control_register.sprite_pattern_address() + (tile * 16) + (uint16_t)row;
    } else {
        if (attributes & 0x80) {
            row = 15 - row;
        }
        uint16_t table = tile & 1;
        tile &= 0xFE;
        if (row > 7) {
            tile += 1;
            row -= 8;
        }
        address = 0x1000 * table + (tile * 16) + (uint16_t)row;
    }
    uint32_t a = ((attributes & 3) << 2);
    uint32_t lt_byte = ppu_data_buffer.read(address);
    uint32_t ht_byte = ppu_data_buffer.read(address + 8);
    uint32_t result = 0;
    for (int j = 0; j < 8; ++j) {
        uint8_t p1;
        uint8_t p2;
        if ((attributes & 0x40) == 0x40) {
            p1 = (lt_byte & 1) << 0;
            p2 = (ht_byte & 1) << 1;
            lt_byte >>= 1;
            ht_byte >>= 1;
        } else {
            p1 = (lt_byte & 0x80) >> 7;
            p2 = (ht_byte & 0x80) >> 6;
            lt_byte <<= 1;
            ht_byte <<= 1;
        }
        result = (result << 4) | (a | p1 | p2);
    }
    return result;
}

void PPU::evaluate_sprites() {
    const int sz = static_cast<int>(control_register.sprite_size());

    int count = 0;

    for (int i = 0; i < 64; i++) {
        const auto sprite = static_cast<uint16_t>(i);
        const uint8_t y = oam_data[(sprite * 4 + 0) % 0x100];
        const uint8_t a = oam_data[(sprite * 4 + 2 ) % 0x100];
        const uint8_t _x = oam_data[(sprite * 4 + 3) % 0x100];

        const int16_t row = static_cast<int16_t>(scanline) - static_cast<int16_t>(y);

        if (row < 0 || row >= sz) {
            continue;
        }

        if (count < 8) {
            sprite_patterns[count] = fetch_sprite_pattern(sprite, row);
            sprite_positions[count] = _x;
            sprite_priorities[count] = (a >> 5) & 1;
            sprite_indexes[count] = i;
        }

        count++;
    }

    if (count > 8) {
        count = 8;
        statusRegister.sprite_overflow();
    }

    sprite_count = count;
}

void PPU::render_pixel() {
    int _x = dot - 1;
    int y = scanline;

    uint8_t background = get_background_pixel().value_or(0);
    auto [i, sprite] = get_sprite_pixel().value_or(std::make_pair(0, 0));
    uint8_t sprite_pixel = sprite;

    if (_x < 8 && !mask_register.background_left()) {
        background = 0;
    }

    if (_x < 8 && !mask_register.sprites_left()) {
        sprite_pixel = 0;
    }

    bool b = background % 4 != 0;
    bool s = sprite_pixel % 4 != 0;

    uint16_t address_low_nyb;

    if (!b && !s) { // Neither background nor sprite is visible
        address_low_nyb = 0;
    } else if (!b) { // Background is not visible
        address_low_nyb = sprite_pixel | 0x10;
    } else if (!s) { // Sprite is not visible
        address_low_nyb = background;
    } else { // Both background and sprite are visible
        if (sprite_indexes[i] == 0 && _x < 255) { // Sprite 0 is visible and at leftmost pixel
            statusRegister.set_status_register(statusRegister.get_status_register() | SPRITE_ZERO_HIT);
        }

        if (sprite_priorities[i] == 0) { // Sprite has priority
            address_low_nyb = sprite_pixel | 0x10;
        } else { // Background has priority
            address_low_nyb = background;
        }
    }

    uint16_t address = 0x3f00 | address_low_nyb;
    uint8_t palette_index = ppu_data_buffer.read(address) % 64;
    PALETTE_COLOR color = SYSTEM_PALETTE[palette_index];

    frame[y*256*3 + _x*3] = color.r;
    frame[y*256*3 + _x*3 + 1] = color.g;
    frame[y*256*3 + _x*3 + 2] = color.b;
}

uint8_t PPU::get_nametable_byte() {
    uint16_t address = 0x2000 | (ppu_address & 0x0fff);
    return ppu_data_buffer.read(address);
}

uint8_t PPU::get_attribute_table_byte() {
    uint16_t addr = 0x23c0 | (ppu_address & 0x0c00) | ((ppu_address >> 4) & 0x38) | ((ppu_address >> 2) & 0x07);
    uint8_t attrbyte = ppu_data_buffer.read(addr);
    uint8_t shift = ((ppu_address >> 4) & 4) | (ppu_address & 2);
    return ((attrbyte >> shift) & 3) << 2;
}



uint8_t PPU::get_background_byte(bool low){
    uint8_t fine_y = (ppu_address >> 12) & 0x07;
    uint16_t tile = name_table_byte;
    uint16_t addr = control_register.background_pattern_address() + fine_y + (16 * tile);

    if(low){
        return ppu_data_buffer.read(addr);
    }else{
        return ppu_data_buffer.read(addr + 8);
    }
}

void PPU::store_tile_data() {
    uint32_t data = 0;
    for (int i = 0; i < 8; ++i) {
        uint8_t p1 = (low_tile_byte & 0x80) >> 7;
        uint8_t p2 = (high_tile_byte & 0x80) >> 6;

        low_tile_byte <<= 1;
        high_tile_byte <<= 1;

        uint8_t a = attribute_table_byte;
        uint32_t b = (a | p1 | p2);

        data = (data << 4) | b;
    }

    tile_data_low |= (data);
}

void PPU::tick(ResultFrame &result) {
    if (--nmi_delay == 0 && nmi_output && nmi_occurred) {
        result.nmi = true;
    }

    if (rendering_enabled() && odd_frame && scanline == 261 && dot == 339) {
        dot = 0;
        scanline = 0;
        odd_frame = false;
        return;
    }

    if (++dot == 341) {
        dot = 0;
        if (++scanline > 261) {
            scanline = 0;
            odd_frame = !odd_frame;
        }
    }
}

ResultFrame PPU::step() {
    ResultFrame result{};
    result.nmi = false;
    result.frame_complete = false;

    tick(result);

    bool pre_line          = scanline == 261;
    bool visible_line      = scanline <= 239;
    bool render_line       = pre_line || visible_line;
    // _post_render_line = scanline == 240;

    bool pre_fetch_cycle = dot >= 321 && dot <= 336;
    bool visible_cycle   = dot >= 1   && dot <= 256;
    bool fetch_cycle     = pre_fetch_cycle || visible_cycle;

    // background logic
    if (rendering_enabled()) {
        if (visible_line && visible_cycle) {
            render_pixel();
        }

        if (render_line && fetch_cycle) {
            tile_data_high <<= 4;

            // Move the lowest 4 bits of tile_data_low to the highest 4 bits of tile_data_high
            tile_data_high |= (tile_data_low >> 28);

            // Perform left shift by 4 on the low bits
            tile_data_low <<= 4;

            switch (dot % 8) {
                case 1: {
                    uint8_t b = get_nametable_byte();
                    name_table_byte = b;
                    break;
                }
                case 3: {
                    uint8_t b = get_attribute_table_byte();
                    attribute_table_byte = b;
                    break;
                }
                case 5: {
                    uint8_t b = get_background_byte(true);
                    low_tile_byte = b;
                    break;
                }
                case 7: {
                    uint8_t b = get_background_byte(false);
                    high_tile_byte = b;
                    break;
                }
                case 0:
                    store_tile_data();
                    break;
                default:
                    break;
            }
        }

        if (pre_line && dot >= 280 && dot <= 304) {
            ppu_address = (ppu_address & 0x841f) | (t & 0x7be0);
        }

        if (render_line) {
            if (fetch_cycle && dot % 8 == 0) {
                increment_horizontal_scroll();
            }

            if (dot == 256) {
                increment_vertical_scroll();
            }

            if (dot == 257) {
                ppu_address = (ppu_address & 0xfbe0) | (t & 0x041f);
            }
        }
    }

    if (rendering_enabled() && dot == 257) {
        if (visible_line) {
            evaluate_sprites();
        } else {
            sprite_count = 0;
        }
    }


    if (scanline == 241 && dot == 1) {
        nmi_occurred = true;
        is_nmi();

        result.frame_complete = true;
        return result;
    }

    if (pre_line && dot == 1) {
        statusRegister.clear_sprite_zero_hit();
        statusRegister.clear_sprite_overflow();

        nmi_occurred = false;
        is_nmi();
    }

    return result;
}


uint8_t PPU_Data::read(uint16_t address) {

    switch (address) {
        case 0x0000 ... 0x1fff:
            return mapper->read(address);
        case 0x2000 ... 0x3eff: {
            auto mirrored_address = name_table_mirroring(address);
            return nametables[mirrored_address];
        }
        case 0x3f00 ... 0x3fff: {
            auto temp = address % 0x20;

            switch (temp & 0x00ff) {
                case 0x10:
                case 0x14:
                case 0x18:
                case 0x1c:
                    temp &= 0xff0f;
                    break;
                default:
                    break;
            }

            return palette_tables[temp];
        }
        default:
            throw std::invalid_argument("bad PPU address " + std::to_string(address));
    }
}

int PPU_Data::name_table_mirroring(uint16_t address){
    address = (address - 0x2000) % 0x1000;
    auto table = address / 0x400;
    auto offset = address % 0x400;
    auto index = 0x2000 + mapper->coefficients()[table] * 0x400 + offset;
    return index % 0x1000;
}

void PPU_Data::write(uint16_t address, uint8_t data) {
    address = address % 0x4000;
    switch (address) {
        case 0x0000 ... 0x1fff:
            mapper->write(address, data);
            break;
        case 0x2000 ... 0x3eff: {
            auto mirrored_address = name_table_mirroring(address);
            nametables[mirrored_address] = data;
            break;
        }
        case 0x3f00 ... 0x3fff: {
            auto i = address % 0x20;
            switch (i & 0x00ff) {
                case 0x10:
                case 0x14:
                case 0x18:
                case 0x1c:
                    i &= 0xff0f;
                    break;
            }
            palette_tables[i] = data;
            break;
        }
        default:
            throw std::invalid_argument("bad PPU address " + std::to_string(address));
    }

}

PPU_Data::PPU_Data(Mapper *mapper) {
    this->mapper = mapper;
    nametables.fill(0);
    palette_tables = {
    };
}

PPU_Data::PPU_Data() {
    nametables.fill(0);
    palette_tables = {
    };

}
PALETTE_COLOR::PALETTE_COLOR(int i, int i1, int i2) {
    r = i;
    g = i1;
    b = i2;

}

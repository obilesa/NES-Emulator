//
// Created by danci on 13/03/2023.
//

#include "Mapper.h"

std::array<int, 4> Mapper::coefficients() {
    switch (mirroring_mode) {
        case mirroring::HORIZONTAL:
            return {0, 0, 1, 1};
        case mirroring::VERTICAL:
            return {0, 1, 0, 1};
        case mirroring::SINGLE0:
            return {0, 0, 0, 0};
        case mirroring::SINGLE1:
            return {1, 1, 1, 1};
        case mirroring::FOUR_SCREEN:
            return {0, 1, 2, 3};
    }
}

mirroring Mapper::get_mirroring() {
    return mirroring_mode;
}

mirroring Mapper::from_hv01(uint8_t mode) {
    switch (mode) {
        case 0:
            return mirroring::HORIZONTAL;
        case 1:
            return mirroring::VERTICAL;
        case 2:
            return mirroring::SINGLE0;
        case 3:
            return mirroring::SINGLE1;
        case 4:
            return mirroring::FOUR_SCREEN;
        default:
            throw std::runtime_error("Invalid mirroring mode");
    }
}

mirroring Mapper::from_vh01(uint8_t mode) {
    switch (mode) {
        case 0:
            return mirroring::VERTICAL;
        case 1:
            return mirroring::HORIZONTAL;
        case 2:
            return mirroring::SINGLE0;
        case 3:
            return mirroring::SINGLE1;
        case 4:
            return mirroring::FOUR_SCREEN;
        default:
            throw std::runtime_error("Invalid mirroring mode");
    }
}

Mapper::Mapper() {

}

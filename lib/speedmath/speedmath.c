#include "speedmath.h"

bcd32_t hex2bcd(uint16_t x) {
    bcd32_t ret = 0;
    while (x > 0) {
        uint16_t d = x % 10;
        x /= 10;
        ret *= 10;
        ret += d;
    }
    return ret;
}

uint16_t mulBy10(uint16_t x) {
    return x * 10;
}

uint16_t divBy10(uint16_t x) {
    return x / 10;
}

uint16_t modBy10(uint16_t x) {
    return x % 10;
}

uint16_t mulBy100(uint16_t x) {
    return x * 100;
}

uint16_t divBy100(uint16_t x) {
    return x / 100;
}

uint16_t modBy100(uint16_t x) {
    return x % 100;
}

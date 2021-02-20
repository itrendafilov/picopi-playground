#include "speedmath.h"

bcd32_t hex2bcd(uint16_t x) {
    bcd32_t ret = 0;
    uint16_t divider = 10000; // 65535 max, i.e. 10^5
    uint32_t coeff = 65536; // 16^5
    while (x > 0) {
        uint16_t d = x / divider;
        x %= divider;
        ret += d * coeff;
        divider /= 10;
        coeff /= 16;
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

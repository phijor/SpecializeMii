#include "crc.h"

#include <inttypes.h>
#include <stddef.h>

uint16_t crc_crc16_ccitt(void *buf, size_t len, uint32_t starting_val)
{
    if (buf == NULL) {
        return -1;
    }

    uint8_t *cbuf = buf;
    uint32_t crc  = starting_val;

    const uint16_t POLY = 0x1021;

    for (size_t i = 0; i < len; i++) {
        for (int bit = 7; bit >= 0; bit--) {
            crc = ((crc << 1) | ((cbuf[i] >> bit) & 0x1)) ^
                  (crc & 0x8000 ? POLY : 0);
        }
    }

    for (int _ = 0; _ < 16; _++) {
        crc = (crc << 1) ^ (crc & 0x8000 ? POLY : 0);
    }

    return (uint16_t)(crc & 0xffff);
}

#ifndef CRC_H
#define CRC_H

#include <inttypes.h>
#include <stddef.h>

uint16_t crc_crc16_ccitt(void *buf, size_t len, uint32_t starting_val);

#endif /* ----- #ifndef CRC_H ----- */

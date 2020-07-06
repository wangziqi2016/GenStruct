
#include "common.h"

uint8_t mask8_low_1[] = {
  0x0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff,
};

uint8_t mask8_high_1[] = {
  0x0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff,
};

uint8_t mask8_1[] = {
  0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
};

void bitsprint8(char *buf, uint8_t value, int dir) {
  assert(dir == BITSPRINT_LE || dir == BITSPRINT_BE);
  uint8_t mask = (dir == BITSPRINT_LE) ? 0x01 : 0x80;
  for(int i = 0;i < 8;i++) {
    buf[i] = (value & mask) ? '1' : '0';
    mask = (dir == BITSPRINT_LE) ? mask << 1 : mask >> 1 ;
  }
  return;
}
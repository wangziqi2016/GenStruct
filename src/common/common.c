
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

// This function is only used for debugging; Will report error if malformed
uint8_t bit8_gen(const char *s) {
  int len = strlen(s);
  if(len < 1 || len > 8) error_exit("Invalid bit string length (see %d)\n", len);
  uint8_t value = 0x00;
  int shift = len - 1;
  while(*s) {
    if(*s != '0' && *s != '1') error_exit("Unexpected char: 0x%02X\n", *s);
    if(*s  == '1') value |= (0x1 << shift);
    shift--;
    s++;
  }
  assert(shift == -1);
  return value;
}

void bitsprint8(char *buf, uint8_t value, int dir) {
  assert(dir == BITSPRINT_LE || dir == BITSPRINT_BE);
  uint8_t mask = (dir == BITSPRINT_LE) ? 0x01 : 0x80;
  for(int i = 0;i < 8;i++) {
    buf[i] = (value & mask) ? '1' : '0';
    mask = (dir == BITSPRINT_LE) ? mask << 1 : mask >> 1 ;
  }
  buf[8] = '\0';
  return;
}

//* File

int file_rem(FILE *fp) {
  int curr = ftell(fp);
  fseek(fp, 0, SEEK_END);
  int end = ftell(fp);
  fseek(fp, curr, SEEK_SET);
  return end - curr;
}

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

uint64_t mask64_low_1[65] = {
  0x0UL, 
  0x1UL, 0x3UL, 0x7UL, 0xfUL, 0x1fUL, 0x3fUL, 0x7fUL, 0xffUL,
  0x1ffUL, 0x3ffUL, 0x7ffUL, 0xfffUL, 0x1fffUL, 0x3fffUL, 0x7fffUL, 0xffffUL,
  0x1ffffUL, 0x3ffffUL, 0x7ffffUL, 0xfffffUL, 0x1fffffUL, 0x3fffffUL, 0x7fffffUL, 0xffffffUL,
  0x1ffffffUL, 0x3ffffffUL, 0x7ffffffUL, 0xfffffffUL, 0x1fffffffUL, 0x3fffffffUL, 0x7fffffffUL, 0xffffffffUL,
  0x1ffffffffUL, 0x3ffffffffUL, 0x7ffffffffUL, 0xfffffffffUL, 0x1fffffffffUL, 0x3fffffffffUL, 0x7fffffffffUL, 0xffffffffffUL,
  0x1ffffffffffUL, 0x3ffffffffffUL, 0x7ffffffffffUL, 0xfffffffffffUL, 
  0x1fffffffffffUL, 0x3fffffffffffUL, 0x7fffffffffffUL, 0xffffffffffffUL,
  0x1ffffffffffffUL, 0x3ffffffffffffUL, 0x7ffffffffffffUL, 0xfffffffffffffUL, 
  0x1fffffffffffffUL, 0x3fffffffffffffUL, 0x7fffffffffffffUL, 0xffffffffffffffUL,
  0x1ffffffffffffffUL, 0x3ffffffffffffffUL, 0x7ffffffffffffffUL, 0xfffffffffffffffUL, 
  0x1fffffffffffffffUL, 0x3fffffffffffffffUL, 0x7fffffffffffffffUL, 0xffffffffffffffffUL,
};

uint64_t bit_gen(const char *s, int bits) {
  int len = strlen(s);
  if(bits > 64 || bits < 1) error_exit("bits must be between [1, 64] (see %d)\n", bits);
  if(len < 1 || len > bits) error_exit("Invalid bit string length (expect [1, %d], see %d)\n", bits, len);
  uint64_t value = 0UL;
  int shift = len - 1;
  while(*s) {
    if(*s != '0' && *s != '1') error_exit("Unexpected char: 0x%02X\n", *s);
    if(*s  == '1') value |= (0x1UL << shift);
    shift--;
    s++;
  }
  assert(shift == -1);
  return value;
}

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

int fp_rem(FILE *fp) {
  int curr = ftell(fp);
  fseek(fp, 0, SEEK_END);
  int end = ftell(fp);
  fseek(fp, curr, SEEK_SET);
  return end - curr;
}

int file_size(const char *name) {
  struct stat buf;
  stat(name, &buf);
  return (int)(buf.st_size);
}

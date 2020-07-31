
#include "common.h"

// This mask array is shared both mask8, 32 and 64
uint64_t mask64_1[65] = {
  0x1UL, 0x2UL, 0x4UL, 0x8UL, 
  0x10UL, 0x20UL, 0x40UL, 0x80UL,
  0x100UL, 0x200UL, 0x400UL, 0x800UL, 
  0x1000UL, 0x2000UL, 0x4000UL, 0x8000UL,
  0x10000UL, 0x20000UL, 0x40000UL, 0x80000UL, 
  0x100000UL, 0x200000UL, 0x400000UL, 0x800000UL,
  0x1000000UL, 0x2000000UL, 0x4000000UL, 0x8000000UL, 
  0x10000000UL, 0x20000000UL, 0x40000000UL, 0x80000000UL,
  0x100000000UL, 0x200000000UL, 0x400000000UL, 0x800000000UL, 
  0x1000000000UL, 0x2000000000UL, 0x4000000000UL, 0x8000000000UL,
  0x10000000000UL, 0x20000000000UL, 0x40000000000UL, 0x80000000000UL, 
  0x100000000000UL, 0x200000000000UL, 0x400000000000UL, 0x800000000000UL,
  0x1000000000000UL, 0x2000000000000UL, 0x4000000000000UL, 0x8000000000000UL, 
  0x10000000000000UL, 0x20000000000000UL, 0x40000000000000UL, 0x80000000000000UL,
  0x100000000000000UL, 0x200000000000000UL, 0x400000000000000UL, 0x800000000000000UL, 
  0x1000000000000000UL, 0x2000000000000000UL, 0x4000000000000000UL, 0x8000000000000000UL, 
  0x0UL, // This is necessary to process benevolent overflows
};

uint64_t bit_gen(const char *s, int bits) {
  int len = strlen(s);
  if(bits > 64 || bits < 1) error_exit("Number of bits must be between [1, 64] (see %d)\n", bits);
  if(len < 1 || len > bits) error_exit("Invalid bit string length (expect [1, %d], see %d)\n", bits, len);
  uint64_t value = 0UL;
  int shift = len - 1; // Shift is between 0 and 63 inclusive
  while(*s) {
    if(*s != '0' && *s != '1') error_exit("Unexpected char: 0x%02X\n", *s);
    if(*s  == '1') value |= (0x1UL << shift);
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

#ifndef MD5_INCLUDED
#define MD5_INCLUDED

#include "stm32f10x.h"

// MD5 context.
typedef struct {
  uint32_t state[4];            // state (ABCD)
  uint32_t count[2];            // number of bits, modulo 2^64 (lsb first)
  uint8_t  buffer[64];          // input buffer
} MD5_CTX;

void MD5Init (MD5_CTX *);
void MD5Update (MD5_CTX *, const uint8_t *, uint32_t);
void MD5Final (uint8_t [16], MD5_CTX *);

#endif // MD5_INCLUDED

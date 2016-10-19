#pragma once

#include "common.h"

typedef struct {
   unsigned char data[64];
   uint datalen;
   uint bitlen[2];
   uint state[8];
} SHA256_CTX;

void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, unsigned char data[], uint len);
void sha256_final(SHA256_CTX *ctx, unsigned char hash[]);
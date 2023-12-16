#pragma once

#ifndef __PRNG_H
#define __PRNG_H

#include "defines.h"

#include <cryptoTools/Crypto/AES.h>

// expand s \in {0, 1}^127 to 256 bits
void PRG(osuCrypto::block s, osuCrypto::block *rand);

void HalfPRG(osuCrypto::block s, osuCrypto::block& rand, uint8_t b);

#endif
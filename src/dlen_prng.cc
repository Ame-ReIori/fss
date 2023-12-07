#include "dlen_prng.h"

void prg(osuCrypto::block s, osuCrypto::block *rand) {
  osuCrypto::AES aes;
  osuCrypto::block tmp[2];
  aes.setKey(s << 1);

  aes.ecbEncCounterMode(osuCrypto::ZeroBlock, tmp);
  memcpy(rand, tmp, 32);
}
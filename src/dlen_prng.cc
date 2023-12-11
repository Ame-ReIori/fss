#include "dlen_prng.h"

void prg(osuCrypto::block s, osuCrypto::block *rand) {
  osuCrypto::AES aes;
  aes.setKey(s << 1);

  aes.ecbEncBlock(osuCrypto::ZeroBlock, rand[0]);
  aes.ecbEncBlock(osuCrypto::OneBlock, rand[1]);
}
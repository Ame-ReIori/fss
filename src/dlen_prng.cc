#include "dlen_prng.h"

const osuCrypto::block pt[2] = { osuCrypto::ZeroBlock, osuCrypto::OneBlock };

void PRG(osuCrypto::block s, osuCrypto::block *rand) {
  osuCrypto::AES aes(s);

  aes.ecbEncBlocksInline<2>(pt, rand);
}

void HalfPRG(osuCrypto::block s, osuCrypto::block& rand, uint8_t b) {
  osuCrypto::AES aes(s);

  aes.ecbEncBlock(pt[b], rand);
}
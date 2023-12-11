/**
 * implement DPF in BGI16
*/

#pragma once

#ifndef __DPF_H
#define __DPF_H

#include "fss.h"
#include "dlen_prng.h"

#include <cryptoTools/Crypto/PRNG.h>
#include <cryptoTools/Common/block.h>

#include <cryptoTools/Common/Timer.h>

// TODO: support template like template <typename GroupEle>
struct DPFKey {
  osuCrypto::block s;
  osuCrypto::block *scw;
  uint8_t *tcw_l;
  uint8_t *tcw_r;
  uint64_t fcw; // the final cw, it is a group element
};

// TODO: support Group Element template
// !only support two parties
// !only support 128-bit security parameter now
// implement [BGI16]
class DPF: public FSS<DPFKey, uint64_t> {
public:
  // we suppose the default security parameter is 127
  DPF(uint8_t bit_length_): FSS(127, bit_length_) {}

  ~DPF() {}

  void InitKey(DPFKey *key);

  uint64_t Convert(osuCrypto::block s);

  void Gen(uint64_t alpha, uint64_t beta, DPFKey *key);

  uint64_t Eval(uint8_t b, DPFKey key, uint64_t input);

  void FreeKey(DPFKey key);
};

void TestDPF();

#endif


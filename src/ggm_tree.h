#pragma once
#ifndef __GGM_TREE_H
#define __GGM_TREE_H

#include "defines.h"
#include "fss.h"
#include "dpf.h"

#include <cryptoTools/Common/block.h>

struct GGMNode {
  osuCrypto::block word;
  GGMNode *l;
  GGMNode *r;
};

// !only support 128-bit security parameter now
class GGMTree {
public:
  uint8_t depth;
  GGMNode *root;

  GGMTree(uint8_t depth_): depth(depth_) {};

  GGMTree(DPF dpf) {
    this->depth = dpf.bit_length;
  }
  
  void FromDPFKey(DPFKey key, uint8_t b);

  GGMTree& operator>>(const GGMTree& rhs);
};

void ReconstrcutTwoGGMTree(GGMTree t0, GGMTree t1);

void TestGGM();

#endif
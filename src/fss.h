#pragma once

#ifndef __FSS_H
#define __FSS_H

#include "defines.h"

template<typename FSSKey, typename GroupEle>
class FSS {
public:
  uint8_t lambda; // security parameter
  uint8_t bit_length; // bit length of input, which should be less than 64

  FSS(uint8_t lambda_, uint8_t bit_length_): lambda(lambda_), bit_length(bit_length_) {}
  ~FSS() {}

  virtual void Gen(uint64_t alpha, GroupEle beta, FSSKey *key) = 0;
  virtual GroupEle Eval(uint8_t b, FSSKey key, uint64_t input) = 0;

  /**
   * ugly implement of EvalAll
   * it accesses nodes except leaves repeatedly
   * thus it has a extremely bad performance which is O(2^n) in detail
   * 
   * **you should** implement it with a more efficient way like
   * accessing all nodes in the GGM Tree for only one time
  */
  GroupEle* EvalAll(uint8_t b, FSSKey key) {
    uint64_t num_input = 1 << bit_length;
    GroupEle *res = (GroupEle *)malloc(num_input * sizeof(GroupEle));

    for (int i = 0; i < num_input; i++) {
      res[i] = Eval(b, key, i);
    }

    return res;
  }

};

#endif
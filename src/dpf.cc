#include "dpf.h"

uint64_t DPF::Convert(osuCrypto::block s) {
  return *(uint64_t *)&s;
}

void DPF::InitKey(DPFKey *key) {
  // only two keys
  key[0].scw = (osuCrypto::block *)malloc(sizeof(osuCrypto::block) * this->bit_length);
  key[0].tcw_l = (uint8_t *)malloc(this->bit_length);
  key[0].tcw_r = (uint8_t *)malloc(this->bit_length);

  key[1].scw = (osuCrypto::block *)malloc(sizeof(osuCrypto::block) * this->bit_length);
  key[1].tcw_l = (uint8_t *)malloc(this->bit_length);
  key[1].tcw_r = (uint8_t *)malloc(this->bit_length);
  
}

void DPF::Gen(uint64_t alpha, uint64_t beta, DPFKey *key) {
  osuCrypto::block s0_, s1_;
  osuCrypto::block double_len_s[2][2];
  osuCrypto::block scw;

  uint8_t t0_, t1_;
  uint8_t keep, lose;
  uint8_t tcw[2], t0[2], t1[2];

  uint8_t cur_bit;
  uint64_t mask = 1 << (this->bit_length - 1); // used to get current processing bit

  uint64_t fcw; // the final cw

  s0_ = osuCrypto::sysRandomSeed();
  s1_ = osuCrypto::sysRandomSeed();
  t0_ = 0;
  t1_ = 1;

  key[0].s = s0_;
  key[1].s = s1_;

  for (int i = 0; i < this->bit_length; i++) {
    prg(s0_, double_len_s[0]);
    prg(s1_, double_len_s[1]);

    // TODO: use better bit extraction algorithm
    cur_bit = (alpha >> (this->bit_length - i - 1)) & 1;// (alpha & (mask >> i)) >> (this->bit_length - i - 1);

    if (cur_bit) {
      keep = 1;
      lose = 0;
    } else {
      keep = 0;
      lose = 1;
    }

    scw = (double_len_s[0][lose] ^ double_len_s[1][lose]) >> 1; // reduce t
    
    t0[0] = *((uint8_t *)&double_len_s[0][0]) & 1;
    t0[1] = *((uint8_t *)&double_len_s[0][1]) & 1;
    t1[0] = *((uint8_t *)&double_len_s[1][0]) & 1;
    t1[1] = *((uint8_t *)&double_len_s[1][1]) & 1;

    tcw[0] = t0[0] ^ t1[0] ^ cur_bit ^ 1;
    tcw[1] = t0[1] ^ t1[1] ^ cur_bit;

    key[0].scw[i] = scw;
    key[0].tcw_l[i] = tcw[0];
    key[0].tcw_r[i] = tcw[1];

    key[1].scw[i] = scw;
    key[1].tcw_l[i] = tcw[0];
    key[1].tcw_r[i] = tcw[1];

    if (t0_) {
      s0_ = (double_len_s[0][keep] >> 1) ^ scw;
      t0_ = t0[keep] ^ tcw[keep];
    } else {
      s0_ = double_len_s[0][keep] >> 1;
      t0_ = t0[keep];
    }

    if (t1_) {
      s1_ = (double_len_s[1][keep] >> 1) ^ scw;
      t1_ = t1[keep] ^ tcw[keep];
    } else {
      s1_ = double_len_s[1][keep] >> 1;
      t1_ = t1[keep];
    }
  }

  fcw = beta - Convert(s0_) + Convert(s1_);
  if (t1_) fcw *= -1;

  key[0].fcw = fcw;
  key[1].fcw = fcw;
}

uint64_t DPF::Eval(uint8_t b, DPFKey key, uint64_t input) {
  osuCrypto::block s_;
  osuCrypto::block double_len_s[2];
  osuCrypto::block rec_cw[2];
  osuCrypto::block s[2];

  uint8_t t_ = b;
  uint8_t t[2];

  uint8_t cur_bit;
  uint64_t mask = 1 << (this->bit_length - 1); // used to get current processing bit

  uint64_t output;

  s_ = key.s;

  for (int i = 0; i < this->bit_length; i++) {
    prg(s_, double_len_s);
    if (t_) {
      s[0] = (double_len_s[0] >> 1) ^ key.scw[i];
      s[1] = (double_len_s[1] >> 1) ^ key.scw[i];
      t[0] = (*((uint8_t *)&double_len_s[0]) & 1) ^ key.tcw_l[i];
      t[1] = (*((uint8_t *)&double_len_s[1]) & 1) ^ key.tcw_r[i];
    } else {
      s[0] = (double_len_s[0] >> 1);
      s[1] = (double_len_s[1] >> 1);
      t[0] = (*((uint8_t *)&double_len_s[0]) & 1);
      t[1] = (*((uint8_t *)&double_len_s[1]) & 1);
    }

    cur_bit = (input >> (this->bit_length - i - 1)) & 1; // (input & (mask >> i)) >> (this->bit_length - i - 1);
    s_ = s[cur_bit];
    t_ = t[cur_bit];
  }

  output = Convert(s_);
  if (t_) {
    output += key.fcw;
  }

  if (b) {
    output *= -1;
  }
  return output;
}

void test_dpf() {
  uint64_t alpha = 3;
  uint64_t beta = 2;

  DPFKey key[2];
  DPF dpf(16);

  printf("lambda is %d, bit length is %d\n", dpf.lambda, dpf.bit_length);

  dpf.InitKey(key);
  dpf.Gen(alpha, beta, key);

  int times = 1000000;
  osuCrypto::Timer timer;

  auto b = timer.setTimePoint("10^6 dpf eval test");

  for (int i = 0; i < times; i++) {
    uint64_t o0 = dpf.Eval(0, key[0], 12);
    // uint64_t o1 = dpf.Eval(1, key[1], 12);
  }

  auto e = timer.setTimePoint("10^6 dpf eval test");

  std::cout << timer << std::endl;

  // printf("recovered = %lu\n", o0 + o1);
  
}
#include "dpf.h"
#include "ggm_tree.h"

inline uint8_t lsb(osuCrypto::block &b) {
  return _mm_cvtsi128_si64(b) & 1;
}

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
  osuCrypto::block mask 
    = osuCrypto::toBlock(0xffffffffffffffff, 0xfffffffffffffffe);

  uint8_t t0_, t1_;
  uint8_t keep, lose;
  uint8_t tcw[2], t0[2], t1[2];

  uint8_t cur_bit;

  uint64_t fcw; // the final cw

  s0_ = osuCrypto::sysRandomSeed() & mask;
  s1_ = osuCrypto::sysRandomSeed() & mask;
  t0_ = 0;
  t1_ = 1;

  key[0].s = s0_;
  key[1].s = s1_;

  for (int i = 0; i < this->bit_length; i++) {
    PRG(s0_, double_len_s[0]);
    PRG(s1_, double_len_s[1]);

    cur_bit = static_cast<uint8_t>(alpha >> (this->bit_length - i - 1)) & 1;

    keep = cur_bit;
    lose = 1 ^ cur_bit;

    scw = (double_len_s[0][lose] ^ double_len_s[1][lose]) & mask; // reduce t
    
    // extract lsb
    t0[0] = lsb(double_len_s[0][0]);
    t0[1] = lsb(double_len_s[0][1]);
    t1[0] = lsb(double_len_s[1][0]);
    t1[1] = lsb(double_len_s[1][1]);

    // configure tcw
    tcw[0] = t0[0] ^ t1[0] ^ cur_bit ^ 1;
    tcw[1] = t0[1] ^ t1[1] ^ cur_bit;

    // store in key
    key[0].scw[i] = scw;
    key[0].tcw_l[i] = tcw[0];
    key[0].tcw_r[i] = tcw[1];

    key[1].scw[i] = scw;
    key[1].tcw_l[i] = tcw[0];
    key[1].tcw_r[i] = tcw[1];

    s0_ = double_len_s[0][keep] & mask;
    s1_ = double_len_s[1][keep] & mask;

    if (t0_) {
      s0_ ^= scw;
      t0_ = t0[keep] ^ tcw[keep];
    } else {
      t0_ = t0[keep];
    }

    if (t1_) {
      s1_ ^= scw;
      t1_ = t1[keep] ^ tcw[keep];
    } else {
      t1_ = t1[keep];
    }
  }

  fcw = beta - Convert(s0_) + Convert(s1_);
  if (t1_) fcw *= -1;

  key[0].fcw = fcw;
  key[1].fcw = fcw;
}

uint64_t DPF::Eval(uint8_t b, DPFKey key, uint64_t input) {
  osuCrypto::block s_, s;
  osuCrypto::block mask = osuCrypto::toBlock(0xffffffffffffffff, 0xfffffffffffffffe);

  uint8_t t_ = b;
  uint8_t t;

  uint8_t cur_bit;
  uint64_t output;

  s_ = key.s;

  for (int i = 0; i < this->bit_length; i++) {
    cur_bit = static_cast<uint8_t>(input >> (this->bit_length - i - 1)) & 1;
    HalfPRG(s_, s, cur_bit);
    t = lsb(s);
    s &= mask;
    if (t_) {
      s ^= key.scw[i];
      t ^= (cur_bit ? key.tcw_r[i] : key.tcw_l[i]);
    }

    s_ = s;
    t_ = t;
  }

  output = Convert(s_) + (key.fcw * t_);

  if (b) {
    output *= -1;
  }
  return output;
}

void DPF::FreeKey(DPFKey key) {
  free(key.scw);
  free(key.tcw_l);
  free(key.tcw_r);
}

void TestDPF() {
  std::cout << "--------------- DPF TEST ---------------" << std::endl;
  uint64_t alpha = 6;
  uint64_t beta = 1;

  DPFKey key[2];
  DPF dpf(50);

  printf("lambda is %d, bit length is %d\n", dpf.lambda, dpf.bit_length);

  dpf.InitKey(key);
  dpf.Gen(alpha, beta, key);

  uint64_t o0, o1;

  // for (int i = 0; i < 8; i++) {
  //   o0 = dpf.Eval(0, key[0], i);
  //   o1 = dpf.Eval(1, key[1], i);
  //   printf("%lu ", (o0 + o1));
  // }
  // std::cout << std::endl;
// #ifdef PERF_TEST
  int times = 1000000;
  osuCrypto::Timer timer;

  // auto b = timer.setTimePoint("10^6 dpf eval test");
  timer.setTimePoint("start");

  for (int i = 0; i < times; i++) {
    o0 = dpf.Eval(0, key[0], i);
    // uint64_t o1 = dpf.Eval(1, key[1], 12);
  }

  timer.setTimePoint("10^6 dpf eval test finish");

  std::cout << timer << std::endl;
// #endif
}
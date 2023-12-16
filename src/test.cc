#include "test.h"

void TestIntrinsAndShift() {
  osuCrypto::block mask = osuCrypto::toBlock(0xffffffff, 0xfffffffe);
  osuCrypto::block seed = osuCrypto::sysRandomSeed();
  osuCrypto::block seed_;

  uint64_t times = 1ull << 60;

  osuCrypto::Timer timer;

  timer.reset();
  timer.setTimePoint("and");
  for (uint64_t i = 0; i < times; i++) {
    seed_ = seed & mask;
  }
  timer.setTimePoint("and");

  timer.setTimePoint("shift");
  for (uint64_t i = 0; i < times; i++) {
    seed_ = (seed >> 1);
  }
  timer.setTimePoint("shift");

  std::cout << timer << std::endl;
}


int main(int argc, char **argv) {
  TestDPF();
  // TestGGM();

  // TestIntrinsAndShift();
}
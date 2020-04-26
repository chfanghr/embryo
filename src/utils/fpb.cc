//
// Created by 方泓睿 on 2020/4/15.
//

#include <cinttypes>

#include <embryo/utils/fpb.h>

namespace embryo::utils {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
int32_t IntToFb(int32_t x) {
  int32_t e = 0;
  if (x < 8)return x;
  while (x >= (8 << 4)) {
    x = (x + 0xf) >> 4;
    e += 4;
  }
  while (x >= (8 << 1)) {
    x = (x + 1) >> 1;
    ++e;
  }
  return ((e + 1) << 3) | (x - 8);
}

int32_t FbToInt(int32_t x) {
  if (x < 8) return x;
  return ((x & 7) + 8) << uint32_t((x >> 3) - 1);
}
#pragma clang diagnostic pop
}
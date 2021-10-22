//
// Created by Eduard Valeyev on 10/22/21.
//

#ifndef TEST_BENCHMARKS_IDX_H
#define TEST_BENCHMARKS_IDX_H

#include <array>
#include <iostream>

constexpr int NDIM = 1;
static_assert(NDIM == 1);  // for now assume 1d
constexpr int LMAX = 21;

struct Idx {
  int l = 0;
  std::array<int, NDIM> x;

  Idx() noexcept {
    for(auto d=0; d!=NDIM; ++d) x[d] = 0;
  }
  Idx(int l, const std::array<int, NDIM>& x) noexcept : l(l), x(x) {}

  bool operator==(const Idx& other) { return l == other.l && x == other.x; }
};

std::ostream& operator<<(std::ostream& os, const Idx& idx) {
  os << "[" << idx.l << " {" << idx.x[0] << "} ]";
  return os;
}

#endif // TEST_BENCHMARKS_IDX_H

//===-- yeti/foundation/prng.cc ---------------------------*- mode: C++ -*-===//
//
//                 _____               _     _   _
//                |   __|___ _ _ ___ _| |___| |_|_|___ ___
//                |   __| . | | |   | . | .'|  _| | . |   |
//                |__|  |___|___|_|_|___|__,|_| |_|___|_|_|
//
//       This file is distributed under the terms described in LICENSE.
//
//===----------------------------------------------------------------------===//
//
// TODO(mtwilliams): Document the purpose of this file.
//
//===----------------------------------------------------------------------===//

#include "yeti/foundation/prng.h"

#include "yeti/foundation/atomics.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

namespace yeti {
namespace foundation {

// We use xorshift1024* to generate random numbers. Nothing fancy, but it's
// fast. Do not use this for cryptography!
namespace {
  static u64 state_[16];
  static u64 p_;

  // We seed via a static constructor. Don't rely on this in other static constructors.
  static const class YETI_LOCAL Seeder {
   public:
    Seeder() {
      // TODO(mtwilliams): Seed from a better source.
      ::srand(::time(NULL));
      for (u32 s = 0; s <= 16; ++s)
        state_[s] = ((u64)::rand() << 32ull) | (u64)::rand();
    }
  } seeder_;
}

template <> u64 prng() {
  const u64 p0 = atomic::add(&p_, 1);
  const u64 p1 = (p0 + 1) & 15;
  const u64 s0 = atomic::load(&state_[p0 & 15]);
  u64 s1 = state_[p1];
  s1 ^= s1 << 31;
  const u64 s = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30);
  atomic::store(&state_[p1], s);
  return s * 1181783497276652981ull;
}

template <> u64 prng(const u64 min, const u64 max) {
  // See this http://stackoverflow.com/a/6852396 for the reasoning behind this.
  const u64 range = 1 + max - min;
  const u64 buckets = ~0ull / range;
  const u64 limit = buckets * range;

  u64 candidate;
  do {
    candidate = prng<u64>();
  } while (candidate >= limit);

  return min + (candidate / buckets);
}

template <> u32 prng() {
  // HACK(mtwilliams): Probably shouldn't mix down like this.
  const u64 full_width_random_number = prng<u64>();
  return (full_width_random_number >> 32ull) ^
         (full_width_random_number & 0xFFFFFFFFull);
}

template <> u32 prng(const u32 min, const u32 max) {
  return prng<u64>(min, max);
}

template <> f32 prng() {
  return (prng<u32>() / (f32)~0ul);
}

template <> f32 prng(const f32 min, const f32 max) {
  return (prng<u32>() / (f32)(~0ul / (max - min))) + min;
}

template <> f64 prng() {
  return (prng<u64>() / (f64)~0ull);
}

template <> f64 prng(const f64 min, const f64 max) {
  return (prng<u64>() / (f64)(~0ull / (max - min))) + min;
}

} // foundation
} // yeti

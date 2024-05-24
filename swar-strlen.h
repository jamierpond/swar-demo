#include <cstddef>
#include <cstring>
#include <zoo/swar/SWAR.h>
#include "utils.h"

namespace jamie_demo {

template <typename S>
constexpr static auto findEmptyLanes(const S &bytes) noexcept {
  constexpr auto NBits = S::NBits;
  using Type = typename S::type;
  using BS = zoo::swar::BooleanSWAR<NBits, Type>;
  constexpr auto Ones = S{S::LeastSignificantBit};

  // Subtracting 1 from a number which is null will cause a borrow from the lane
  // above which will cause the MSB to be set. This is the key to the algorithm.
  auto firstNullTurnsOnMSB = bytes - Ones;
  auto inverseBytes = ~bytes;

  // If the whole byte was null, the MBS will be set, AND the previous byte
  // will have been zero.
  auto firstMSBsOnIsFirstNull = firstNullTurnsOnMSB & inverseBytes;
  auto onlyMSBs = zoo::swar::convertToBooleanSWAR(firstMSBsOnIsFirstNull).value();
  return BS{onlyMSBs};
}

constexpr static std::size_t c_strLength(const char *s) noexcept {
  using S = zoo::swar::SWAR<8, uint64_t>;
  constexpr auto MSBs = S{S::MostSignificantBit},
                 Ones = S{S::LeastSignificantBit};
  constexpr auto BytesPerIteration = sizeof(S::type); // 8 * Num Lanes = 64
  S initialBytes{0};

  constexpr auto indexOfFirstTrue = [](auto boolSwar) {
    return boolSwar.lsbIndex();
  };

  // Load the first block, calculate misalignment, and zero out the lower
  // part of the block if necessary
  auto [alignedBase, misalignment] = blockAlignedLoad(s, &initialBytes.m_v);
  auto bytes = adjustMisalignmentFor_strlen(initialBytes, misalignment);

  for (;;) {
    auto emptyBytes = findEmptyLanes(bytes);
    if (emptyBytes) {
      return alignedBase + indexOfFirstTrue(emptyBytes) - s;
    }

    // advance
    alignedBase += BytesPerIteration;
    memcpy(&bytes, alignedBase, BytesPerIteration);
  }
}

/* testing */
using S = zoo::swar::SWAR<8, uint64_t>;
constexpr auto null_hello_uint64 = 0x68'65'6c'6c'6f'00'00'00; // Hello\0\0\0
constexpr auto cool_hello_uint64 = 0x68'65'6c'6c'6f'6f'6f'6f; // Helloooo
constexpr auto null_swar = S{null_hello_uint64};
constexpr auto cool_swar = S{cool_hello_uint64};
static_assert(findEmptyLanes(cool_swar).value() == 0x00'00'00'00'00'00'00'00);
static_assert(findEmptyLanes(null_swar).value() == 0x00'00'00'00'00'80'80'80);

} // namespace jamie_demo

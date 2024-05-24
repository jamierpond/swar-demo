#include <cstddef>
#include <cstring>
#include <tuple>
#include <zoo/swar/SWAR.h>

namespace jamie_demo {

/// @brief Loads the "block" containing the pointer, by proper alignment
/// @tparam PtrT Pointer type for loading
/// @tparam Block as the name indicates
/// @param pointerInsideBlock the potentially misaligned pointer
/// @param b where the loaded bytes will be put
/// @return a pair to indicate the aligned pointer to the base of the block
/// and the misalignment, in bytes, of the source pointer
template <typename PtrT, typename Block>
constexpr inline static std::tuple<PtrT *, int>
blockAlignedLoad(PtrT *pointerInsideBlock, Block *b) noexcept {
  uintptr_t asUint = reinterpret_cast<uintptr_t>(pointerInsideBlock);
  constexpr auto Alignment = alignof(Block), Size = sizeof(Block);
  static_assert(Alignment == Size);
  const auto misalignment = asUint % Size;
  auto *base = reinterpret_cast<PtrT *>(asUint - misalignment);
  memcpy(b, base, Size);
  return {base, misalignment};
}

/// \brief Helper function to fix the non-string part of block
template <typename S>
constexpr inline static S
adjustMisalignmentFor_strlen(S data, int misalignment) noexcept {
  // The speculative load has the valid data in the higher lanes.
  // To use the same algorithm as the rest of the implementation, simply
  // populate with ones the lower part, in that way there won't be nulls.
  constexpr typename S::type Zero{0};
  auto zeroesInMisalignedOnesInValid =
           (~Zero)                // all ones
           << (misalignment * 8), // assumes 8 bits per char
      onesInMisalignedZeroesInValid = ~zeroesInMisalignedOnesInValid;
  return data | S{onesInMisalignedZeroesInValid};
}

template <typename S>
constexpr static auto findEmptyLanes(const S &bytes) noexcept {
  constexpr auto NBits = S::NBits;
  using Type = typename S::type;
  using BS = zoo::swar::BooleanSWAR<NBits, Type>;
  constexpr auto Ones = S{S::LeastSignificantBit};
  auto firstNullTurnsOnMSB = bytes - Ones;
  auto inverse = ~bytes;
  auto firstMSBsOnIsFirstNull = firstNullTurnsOnMSB & inverse;
  auto onlyMSBs =
      zoo::swar::convertToBooleanSWAR(firstMSBsOnIsFirstNull).value();
  return BS{onlyMSBs};
}

using S = zoo::swar::SWAR<8, uint64_t>;
constexpr auto null_hello_uint64 = 0x68'65'6c'6c'6f'00'00'00; // Hello\0\0\0
constexpr auto cool_hello_uint64 = 0x68'65'6c'6c'6f'6f'6f'6f; // Helloooo
constexpr auto null_swar = S{null_hello_uint64};
constexpr auto cool_swar = S{cool_hello_uint64};
static_assert(findEmptyLanes(cool_swar).value() == 0x00'00'00'00'00'00'00'00);
static_assert(findEmptyLanes(null_swar).value() == 0x00'00'00'00'00'80'80'80);

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
    alignedBase += BytesPerIteration;
    memcpy(&bytes, alignedBase, BytesPerIteration); // advance
  }
}

} // namespace jamie_demo

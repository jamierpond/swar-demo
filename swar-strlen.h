#include <cstddef>
#include <cstring>
#include <tuple>
#include <type_traits>
#include <zoo/swar/SWAR.h>

namespace jamie_demo {


template <typename IntegerType = size_t>
constexpr static
std::enable_if_t<std::is_integral_v<IntegerType>, bool>
is_power_of_two(IntegerType x) noexcept {
  return x > 0 && (x & (x - 1)) == 0;
}

template <typename IntegerType = size_t, IntegerType X>
constexpr static
std::enable_if_t<std::is_integral_v<IntegerType>, bool>
is_power_of_two() noexcept {
  return is_power_of_two(X);
}
static_assert(is_power_of_two<int, 4>());

template <size_t N>
constexpr static
std::enable_if_t<
    std::is_integral_v<size_t> &&
    is_power_of_two<size_t, N>(), size_t>
modulo_power_of_two(auto x) noexcept {
  return x & (N - 1);
}
static_assert(modulo_power_of_two<4>(0) == 0);
static_assert(modulo_power_of_two<8>(9) == 1);
static_assert(modulo_power_of_two<4096>(4097) == 1);

/// @brief Loads the "block" containing the pointer, by proper alignment
/// @tparam PtrT Pointer type for loading
/// @tparam Block as the name indicates
/// @param pointerInsideBlock the potentially misaligned pointer
/// @param b where the loaded bytes will be put
/// @return a pair to indicate the aligned pointer to the base of the block
/// and the misalignment, in bytes, of the source pointer
template <typename PtrT, typename Block>
constexpr static std::tuple<PtrT *, int> blockAlignedLoad(PtrT *pointerInsideBlock, Block *b) {
  uintptr_t asUint = reinterpret_cast<uintptr_t>(pointerInsideBlock);
  constexpr auto Alignment = alignof(Block), Size = sizeof(Block);
  static_assert(Alignment == Size);
  static_assert(is_power_of_two(Alignment));
  const auto misalignment = modulo_power_of_two<Alignment>(asUint);
  auto *base = reinterpret_cast<PtrT *>(asUint - misalignment);
  memcpy(b, base, Size);
  return {base, misalignment};
}

/// \brief Helper function to fix the non-string part of block
template <typename S>
constexpr static S adjustMisalignmentFor_strlen(S data, int misalignment) {
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

constexpr static std::size_t c_strLength(const char *s) noexcept {
    using S = zoo::swar::SWAR<8, uint64_t>;
    constexpr auto
        MSBs = S{S::MostSignificantBit},
        Ones = S{S::LeastSignificantBit};
    constexpr auto BytesPerIteration = sizeof(S::type);
    S initialBytes{0};

    constexpr auto indexOfFirstTrue = [](auto bs) { return bs.lsbIndex(); };

     // Misalignment must be taken into account because a SWAR read is
    // speculative, it might read bytes outside of the actual string.
    // It is safe to read within the page where the string occurs, and to
    // guarantee that, simply make aligned reads because the size of the SWAR
    // base size will always divide the memory page size
    const auto res = blockAlignedLoad(s, &initialBytes.m_v);
    auto alignedBase = std::get<0>(res);
    const auto misalignment = std::get<1>(res);
    auto bytes = adjustMisalignmentFor_strlen(initialBytes, misalignment);
    for(;;) {
        const auto firstNullTurnsOnMSB = bytes - Ones;
        // The first lane with a null will borrow and set its MSB on when
        // subtracted one.
        // The borrowing from the first null interferes with the subsequent
        // lanes, that's why we focus on the first null.
        // The lanes previous to the first null might keep their MSB on after
        // subtracting one (if their value is greater than 0x80).
        // This provides a way to detect the first null: It is the first lane
        // in firstNullTurnsOnMSB that "flipped on" its MSB
        const auto cheapestInversionOfMSBs = ~bytes;
        const auto firstMSBsOnIsFirstNull =
            firstNullTurnsOnMSB & cheapestInversionOfMSBs;
        auto onlyMSBs = zoo::swar::convertToBooleanSWAR(firstMSBsOnIsFirstNull);
        if(onlyMSBs) {
            return alignedBase + indexOfFirstTrue(onlyMSBs) - s;
        }
        alignedBase += BytesPerIteration;
        memcpy(&bytes, alignedBase, BytesPerIteration);
    }
}


constexpr char hello[] = "Hello";
// static_assert(c_strLength(hello) == 5);

}

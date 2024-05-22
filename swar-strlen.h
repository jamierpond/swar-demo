#include <zoo/swar/SWAR.h>
#include <tuple>
#include <cstring>

/// @brief Loads the "block" containing the pointer, by proper alignment
/// @tparam PtrT Pointer type for loading
/// @tparam Block as the name indicates
/// @param pointerInsideBlock the potentially misaligned pointer
/// @param b where the loaded bytes will be put
/// @return a pair to indicate the aligned pointer to the base of the block
/// and the misalignment, in bytes, of the source pointer
template<typename PtrT, typename Block>
std::tuple<PtrT *, int>
blockAlignedLoad(PtrT *pointerInsideBlock, Block *b) {
    uintptr_t asUint = reinterpret_cast<uintptr_t>(pointerInsideBlock);
    constexpr auto Alignment = alignof(Block), Size = sizeof(Block);
    static_assert(Alignment == Size);
    auto misalignment = asUint % Alignment;
    auto *base = reinterpret_cast<PtrT *>(asUint - misalignment);
    memcpy(b, base, Size);
    return { base, misalignment };
}

/// \brief Helper function to fix the non-string part of block
template<typename S>
S adjustMisalignmentFor_strlen(S data, int misalignment) {
    // The speculative load has the valid data in the higher lanes.
    // To use the same algorithm as the rest of the implementation, simply
    // populate with ones the lower part, in that way there won't be nulls.
    constexpr typename S::type Zero{0};
    auto
        zeroesInMisalignedOnesInValid =
            (~Zero) // all ones
            <<  (misalignment * 8), // assumes 8 bits per char
        onesInMisalignedZeroesInValid = ~zeroesInMisalignedOnesInValid;
    return data | S{onesInMisalignedZeroesInValid};
}

static std::size_t c_strLength_natural(const char *s) {
    using S = zoo::swar::SWAR<8, std::uint64_t>;
    S initialBytes;
    auto [base, misalignment] = blockAlignedLoad(s, &initialBytes.m_v);
    auto bytes = adjustMisalignmentFor_strlen(initialBytes, misalignment);
    for(;;) {
        auto nulls = zoo::swar::equals(bytes, S{0});
        if(nulls) { // there is a null!
            auto firstNullIndex = nulls.lsbIndex();
            return firstNullIndex + base - s;
        }
        base += sizeof(S);
        memcpy(&bytes.m_v, base, 8);
    }
}


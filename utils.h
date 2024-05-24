#include <random>
#include <benchmark/benchmark.h>
#include <zoo/swar/SWAR.h>
#include <atoi-corpus.h> // zoo benchmark util

namespace jamie_demo {

static int g_SideEffect = 0;

template<typename Corpus, typename Callable>
void goOverCorpus(Corpus &c, Callable &&cc) {
    auto iterator = c.commence();
    auto result = g_SideEffect;
    do {
        result ^= cc(*iterator);
    } while(iterator.next());
    g_SideEffect = result;
}

template<typename CorpusMaker, typename Callable>
void runBenchmark(benchmark::State &s) {
    std::random_device rd;
    std::mt19937 g(rd());
    auto corpus = CorpusMaker::makeCorpus(g);
    for(auto _: s) {
        goOverCorpus(corpus, Callable());
        benchmark::ClobberMemory();
    }
}

/* don't worry about this for now */
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

/* don't worry about this for now */
template <typename S>
constexpr inline static S
adjustMisalignmentFor_strlen(S data, int misalignment) noexcept {
  constexpr typename S::type Zero{0};
  auto zeroesInMisalignedOnesInValid =
           (~Zero)                // all ones
           << (misalignment * 8), // assumes 8 bits per char
      onesInMisalignedZeroesInValid = ~zeroesInMisalignedOnesInValid;
  return data | S{onesInMisalignedZeroesInValid};
}

} // namespace jamie_demo

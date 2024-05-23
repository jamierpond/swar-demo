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

} // namespace jamie_demo

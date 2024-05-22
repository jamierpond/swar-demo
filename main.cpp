#include "utils.h"
#include "swar-strlen.h"

constexpr static size_t naieve(const char *s) noexcept {
    size_t i = 0;
    while (s[i] != '\0') {
        i++;
    }
    return i;
}

#define STRLEN_FUNCTIONS \
    X(_LIBC_STRLEN, strlen) \
    X(_ZOO_STRLEN_NORMAL, c_strLength) \
    X(FUN, naieve) \

#define X(Typename, FunctionToCall) \
    struct Invoke##Typename { int operator()(const char *p) { return FunctionToCall(p); } };
    STRLEN_FUNCTIONS
#undef X

#define X(Typename, _) \
    BENCHMARK(runBenchmark<CorpusStringLength, Invoke##Typename>);
    STRLEN_FUNCTIONS
#undef X

BENCHMARK_MAIN();



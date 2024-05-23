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
    X(_ZOO_STRLEN, jamie_demo::c_strLength) \
    X(_NAIEVE_STRLEN, naieve) \
    X(_LIBC_STRLEN, strlen) \

#define X(Typename, FunctionToCall) \
    struct Invoke##Typename { static int operator()(const char *p) { return FunctionToCall(p); } };
    STRLEN_FUNCTIONS
#undef X

#define X(Typename, _) \
    BENCHMARK(runBenchmark<CorpusStringLength, Invoke##Typename>);
    STRLEN_FUNCTIONS
#undef X

BENCHMARK_MAIN();



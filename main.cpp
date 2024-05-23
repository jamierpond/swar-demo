#include "utils.h"
#include "swar-strlen.h"

constexpr inline static size_t naieve(const char *s) noexcept {
    size_t i = 0;
    while (s[i] != '\0') {
        i++;
    }
    return i;
}
static_assert(naieve("hello") == 5, "naieve strlen is broken");

#define STRLEN_FUNCTIONS \
    X(_ZOO_STRLEN, jamie_demo::c_strLength) \
    X(_NAIEVE_STRLEN, naieve) \
    X(_LIBC_STRLEN, strlen) \

#define X(FunctionName, FunctionToCall) \
    struct Invoke##FunctionName { int operator()(const char *p) { return FunctionToCall(p); } };
    STRLEN_FUNCTIONS
#undef X

#define X(Typename, _) \
    BENCHMARK(jamie_demo::runBenchmark<CorpusStringLength, Invoke##Typename>);
    STRLEN_FUNCTIONS
#undef X

BENCHMARK_MAIN();



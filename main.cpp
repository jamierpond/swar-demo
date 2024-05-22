#include "utils.h"
#include "swar-strlen.h"

#define STRLEN_FUNCTIONS \
    X(_LIBC_STRLEN, strlen) \
    X(_ZOO_STRLEN, c_strLength_natural) \
    X(_ZOO_NATURAL_STRLEN, c_strLength_manualComparison) \

#define X(Typename, FunctionToCall) \
    struct Invoke##Typename { int operator()(const char *p) { return FunctionToCall(p); } };
    STRLEN_FUNCTIONS
#undef X

#define X(Typename, _) \
    BENCHMARK(runBenchmark<CorpusStringLength, Invoke##Typename>);
    STRLEN_FUNCTIONS
#undef X

BENCHMARK_MAIN();



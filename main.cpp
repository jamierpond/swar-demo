#include "utils.h"
#include "swar-strlen.h"

struct StrLen { int operator()(const char *p) { return strlen(p); } };

struct StrBetter { int operator()(const char *p) { return c_strLength_natural(p); } };

BENCHMARK(runBenchmark<CorpusLeadingSpaces, StrLen>);
BENCHMARK(runBenchmark<CorpusLeadingSpaces, StrBetter>);

BENCHMARK_MAIN();



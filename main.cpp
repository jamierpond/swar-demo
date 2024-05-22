#include "utils.h"

struct StrLen { int operator()(const char *p) { return strlen(p); } };

BENCHMARK(runBenchmark<CorpusLeadingSpaces, StrLen>);

BENCHMARK_MAIN();



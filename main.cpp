#include "utils.h"
#include "swar-strlen.h"

#define STRLEN_X_LIST \
  X(strlen) \
  X(c_strLength_natural)

struct StrLen { int operator()(const char *p) { return strlen(p); } };
struct StrZooNatural { int operator()(const char *p) { return c_strLength_natural(p); } };
struct StrBetter { int operator()(const char *p) { return c_strLength_manualComparison(p); } };

BENCHMARK(runBenchmark<CorpusLeadingSpaces, StrZooNatural>);
BENCHMARK(runBenchmark<CorpusLeadingSpaces, StrBetter>);
BENCHMARK(runBenchmark<CorpusLeadingSpaces, StrLen>);

BENCHMARK_MAIN();



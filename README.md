stringlen benchmarks 22 May
```
------------------------------------------------------------------------------------------------
Benchmark                                                      Time             CPU   Iterations
------------------------------------------------------------------------------------------------
runBenchmark<CorpusStringLength, InvokeNAIEVE_STRLEN>      32359 ns        32181 ns        21571
runBenchmark<CorpusStringLength, Invoke_LIBC_STRLEN>        4995 ns         4946 ns       125089
runBenchmark<CorpusStringLength, Invoke_ZOO_STRLEN>         4079 ns         4047 ns       173873
```

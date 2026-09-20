# Bounded benchmark run

Date: 2026-09-15

This is a first measured sweep, not the final experiment. It ran 26 of the 29
candidate cases. The 10,000,000-vertex path and cycle and the 6,400-vertex
dense random graph were skipped because their memory requirements had not yet
been checked on this computer.

## Build and measurements

- Compiler: MSYS2 UCRT64 GCC 16.1.0
- Language: C++23
- Build flags: -O2 -static -Wall -Wextra -Wpedantic
- Platform: Windows
- Seed for random and clustered graphs: 14487692
- Timing: one warm-up and seven measured runs per algorithm on one graph
- Memory: one algorithm per fresh process, measured with Windows
  PeakWorkingSetSize after the algorithm finishes
- Process timeout: 120 seconds for each program run
- Safety limits: at most 1,000,000 vertices and 3,000,000 edges per case

The sweep command used the compiled benchmark executable and:

```powershell
python .\scripts\run_benchmarks.py --timeout 120 --max-vertices 1000000 --max-edges 3000000 --executable <path-to-scc_benchmark.exe> --output results\timings-bounded.csv --memory-output results\memory-bounded.csv
```

The timing file contains 364 individual run rows. The memory file contains 52
rows, one for each algorithm in each case. All timing and memory runs agreed
on the number of components for their corresponding case.

Peak working set is total process memory. It includes the stored graph,
program code, and shared pages. It should not be described as the algorithm's
exact extra allocation count. The dense case may be dominated by the memory
needed to generate and store its graph.

The figures can be recreated from these CSV files with
`python scripts/plot_results.py`. `running-time.svg` shows the median of seven
timings for each algorithm and case. `peak-memory.svg` shows the one recorded
peak per algorithm and case. Panels use separate vertical scales and a
logarithmically spaced vertex axis.

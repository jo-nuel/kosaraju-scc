# Empirical study plan

## Question I want to answer

Kosaraju's and Tarjan's algorithms both find strongly connected components in
linear time. This means their running time should grow in proportion to the
number of vertices and edges. However, they organise the work differently.

Kosaraju searches the graph twice and builds a second graph with every edge
reversed. Tarjan searches once, but keeps extra information about the current
search path. I want to find out whether these differences can be seen in
running time and memory use.

## Comparison algorithm

I will implement Tarjan's algorithm as the baseline. It will use the same
`DirectedGraph` class and return the same `SCCResult` format as Kosaraju's
algorithm. Sharing these parts should make the comparison fair and will also
let the existing correctness checks compare both algorithms directly.

## Graph families

One graph shape is not enough to show how the algorithms behave. I plan to
test several shapes:

- A long directed path, where every vertex is its own component
- One large directed cycle, where every vertex is in the same component
- Separate groups with many edges inside each group and only one-way edges
  between groups
- Sparse random graphs, with an average out-degree between 2 and 4
- Dense random graphs, containing 25% to 50% of the possible directed edges

The graph generators will use recorded random seeds. This means the same test
graphs can be created again if a result looks unusual. Timing graphs will not
contain self-loops or repeated edges, because those edges add work without
adding a new connection. Each graph will be built once and passed unchanged to
both algorithms.

Clustered graphs will use equal-sized groups. A directed cycle through each
group guarantees that it is strongly connected before random internal edges
are added. One bridge joins each group to the next group, and every bridge
points forward. This creates one connected graph while preserving the planned
component boundaries.

## Measurements

Graph generation and file reading will happen before timing begins. The timed
section will contain only the component algorithm. Each case will be run more
than once, and I will report the median time so that one slow run has less
effect on the result.

Each algorithm will have one warm-up run followed by seven timed runs. The
warm-up result will not be recorded. During the timed repetitions, the order
of the two algorithms will alternate so one algorithm is not always measured
first. Every individual time will be saved, allowing the median to be checked
again later.

The main measurements will be:

- Number of vertices
- Number of edges
- Graph family and random seed
- Running time for each algorithm
- Total peak process working set while each algorithm runs

Memory will be measured in fresh Windows processes, one algorithm per process.
The same graph settings and seed rebuild equivalent inputs. Peak working set
includes the graph, program, and shared pages, so it is an approximate total
process comparison, not a count of the algorithm's own allocations.

Release builds will be used for the final measurements. Debug checks are
useful while developing, but they would make the timing less representative.

## Correctness checks before timing

Both algorithms must produce the same grouping before their times are
compared. Component numbers themselves may differ, so I will compare whether
every pair of vertices is placed together or apart by both algorithms. Small
graphs can also be checked against the existing mutual-reachability method.

The benchmark will keep a small result from each run, such as the number of
components, so the compiler cannot discard the algorithm call as unused work.

## Initial expectations

I expect both algorithms to show roughly linear growth when vertices and
edges are increased together. Tarjan may use less memory because it does not
build the transposed graph. Kosaraju may still be competitive in running time
because its two searches are simple and visit adjacency lists in a regular
way.

These are predictions rather than conclusions. The purpose of the benchmark
is to see whether the measurements support them and to investigate cases that
do not.

## First implementation step

Add Tarjan's algorithm behind the existing SCC interface. Start with tests on
the hand-traced graph and unusual small cases, then compare it with Kosaraju on
all four-vertex directed graphs.

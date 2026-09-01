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
- Sparse random graphs, where each vertex has only a few outgoing edges
- Denser random graphs, where the number of edges grows faster

The graph generators will use recorded random seeds. This means the same test
graphs can be created again if a result looks unusual.

## Measurements

Graph generation and file reading will happen before timing begins. The timed
section will contain only the component algorithm. Each case will be run more
than once, and I will report the median time so that one slow run has less
effect on the result.

The main measurements will be:

- Number of vertices
- Number of edges
- Graph family and random seed
- Running time for each algorithm
- Extra memory used while each algorithm runs

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

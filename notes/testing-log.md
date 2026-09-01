# Testing log

## Small random graph check

I compared the component groups from Kosaraju's algorithm with a slower check
based on mutual reachability. The slower check tries every pair of vertices.
Two vertices should be in the same component only when each one can reach the
other.

The first run reported a mismatch. The problem was in the checking script,
not the component algorithm. One side of the comparison used `true` and
`false`, while the other side used `1` and `0`. JavaScript treats these as
different when using strict comparison.

I converted the reachability result to a Boolean value and ran the check
again. All 1,000 small random graphs matched.

## Exhaustive four-vertex check

A graph with four vertices has 16 possible directed edges when self-loops are
included. I generated all 65,536 combinations of those edges. For every graph,
I compared Kosaraju's groups with the mutual reachability check.

All 65,536 graphs matched. This also covered empty graphs, self-loops, separate
cycles, one-way connections, and graphs where every possible edge is present.

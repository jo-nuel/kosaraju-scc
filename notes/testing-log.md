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

After adding Tarjan's algorithm, I reused the same exhaustive check instead of
only testing a few examples. For each graph, Tarjan and Kosaraju are each
checked against mutual reachability and then checked against each other. Both
algorithms matched on all 65,536 graphs.

## Long path test

The first version uses recursive searches. A path with 100,000 vertices may
need 100,000 nested function calls before the first vertex can finish. The
result may depend on how much call-stack space is available on the computer.

I added a long one-way path test before changing the search. Every vertex in
this graph should form its own component. The next step is to replace the
recursive calls with a stack stored by the program itself.

The recursive calls were replaced with explicit stacks. The first search
stores both the current vertex and which neighbour should be checked next.
This is needed because a vertex must only be added to the finishing order
after all of its neighbours have been handled. The second search only needs a
stack of vertex numbers because it does not record finishing times.

The first Tarjan version also uses recursive calls. I added the same
100,000-vertex path requirement for Tarjan before changing its implementation.
An equivalent recursive JavaScript check exceeded its call-stack limit. A
native C++ compiler is not available in the current shell, so I have not
claimed this as a C++ test result. The recursive C++ version still has the same
risk and should be replaced with explicit search frames.

I replaced Tarjan's recursive calls with search frames that store a vertex and
the next outgoing edge to inspect. Tarjan still needs a separate active stack,
because that stack records vertices that may belong to the same component.
When a search frame finishes, its low-link value is passed to the parent frame.
This takes the place of the update that previously happened after a recursive
call returned.

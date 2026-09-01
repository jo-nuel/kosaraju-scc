# Initial design notes

## What I am trying to find

The input is a directed graph, which means each connection has a direction.
I need to separate its vertices into groups called strongly connected
components. Inside one of these groups, every vertex can reach every other
vertex by following the directed edges.

Kosaraju's algorithm finds these groups by searching the graph twice. The
first search records when each vertex is finished. The second search uses a
copy of the graph with every edge reversed.

## Small example to trace by hand

I will start with this graph:

```text
0 -> 1
1 -> 2
2 -> 0
2 -> 3
3 -> 4
4 -> 3
4 -> 5
```

The groups I expect are:

```text
{0, 1, 2}
{3, 4}
{5}
```

Vertices 0, 1, and 2 can all reach each other. Vertices 3 and 4 can reach each
other. Vertex 5 cannot return to any earlier vertex, so it belongs by itself.

## First search

Starting at vertex 0 and checking neighbours in the order shown above gives
this route:

```text
0 -> 1 -> 2 -> 3 -> 4 -> 5
```

The search finishes the deepest vertex first. The finishing order should be:

```text
5, 4, 3, 2, 1, 0
```

The second search must therefore consider the vertices in the opposite order:

```text
0, 1, 2, 3, 4, 5
```

## Reversed graph

Reversing each edge gives:

```text
1 -> 0
2 -> 1
0 -> 2
3 -> 2
4 -> 3
3 -> 4
5 -> 4
```

Starting the second search at 0 finds 0, 2, and 1. Starting at 3 then finds 3
and 4. Vertex 5 is left as its own group. This matches the expected result.

## Initial code choices

- Store the graph as a list of outgoing neighbours for each vertex.
- Keep graph creation separate from the component-finding algorithm.
- Return one component number for each vertex and the total number of groups.
- Begin with recursive searches because they match the steps above closely.
- Test a long path before deciding whether recursion should be replaced with a
  manual stack.

## Things I need to test

- An empty graph
- One vertex with no edges
- Several vertices with no edges
- A self-loop
- One large cycle
- A graph with no cycles
- Several separate cycles
- A one-way edge between two components
- Repeated edges between the same vertices
- A long path that may make recursive search unsafe

Component numbers may change depending on search order. The tests should check
which vertices are grouped together instead of requiring particular component
numbers.

## Next step

Create the small C++ project structure and write a basic graph class. Before
implementing the full algorithm, check that directed edges are stored under
the correct starting vertex.

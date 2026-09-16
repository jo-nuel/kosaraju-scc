import argparse
import csv
import io
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


SEED = 14487692
TIMED_RUNS = 7
CSV_FIELDS = [
    "graph_family",
    "vertices",
    "edges",
    "seed",
    "algorithm",
    "run",
    "nanoseconds",
    "components",
]
MEMORY_FIELDS = [
    "graph_family",
    "vertices",
    "edges",
    "seed",
    "algorithm",
    "peak_working_set_bytes",
    "components",
]


@dataclass(frozen=True)
class BenchmarkCase:
    family: str
    arguments: tuple[str, ...]
    description: str


def full_cases() -> list[BenchmarkCase]:
    cases: list[BenchmarkCase] = []

    for vertex_count in [10_000, 100_000, 1_000_000, 10_000_000]:
        cases.append(
            BenchmarkCase("path", ("path", str(vertex_count)),
                          f"path with {vertex_count} vertices")
        )
        cases.append(
            BenchmarkCase("cycle", ("cycle", str(vertex_count)),
                          f"cycle with {vertex_count} vertices")
        )

    for vertex_count in [
        1_000,
        2_000,
        4_000,
        8_000,
        16_000,
        32_000,
        64_000,
        128_000,
        256_000,
    ]:
        edge_count = vertex_count * 4
        cases.append(
            BenchmarkCase(
                "sparse_random",
                ("random", str(vertex_count), str(edge_count), str(SEED)),
                f"sparse random graph with {vertex_count} vertices",
            )
        )

    for vertex_count in [200, 400, 800, 1_600, 3_200, 6_400]:
        edge_count = vertex_count * (vertex_count - 1) // 4
        cases.append(
            BenchmarkCase(
                "dense_random",
                ("random", str(vertex_count), str(edge_count), str(SEED)),
                f"dense random graph with {vertex_count} vertices",
            )
        )

    group_size = 10
    extra_edges_per_group = 30
    for group_count in [10, 20, 40, 80, 160, 320]:
        cases.append(
            BenchmarkCase(
                "clustered",
                (
                    "clustered",
                    str(group_count),
                    str(group_size),
                    str(extra_edges_per_group),
                    str(SEED),
                ),
                f"clustered graph with {group_count} groups",
            )
        )

    return cases


def quick_cases() -> list[BenchmarkCase]:
    wanted = {
        ("path", "10000"),
        ("cycle", "10000"),
        ("random", "1000", "4000", str(SEED)),
        ("random", "200", str(200 * 199 // 4), str(SEED)),
        ("clustered", "10", "10", "30", str(SEED)),
    }
    return [case for case in full_cases() if case.arguments in wanted]


def vertex_count_for(case: BenchmarkCase) -> int:
    if case.family == "clustered":
        return int(case.arguments[1]) * int(case.arguments[2])
    return int(case.arguments[1])


def edge_count_for(case: BenchmarkCase) -> int:
    if case.family == "path":
        return max(0, int(case.arguments[1]) - 1)
    if case.family == "cycle":
        return int(case.arguments[1])
    if case.family == "clustered":
        groups = int(case.arguments[1])
        group_size = int(case.arguments[2])
        extra = int(case.arguments[3])
        return groups * (group_size + extra) + max(0, groups - 1)
    return int(case.arguments[2])


def run_case(
    executable: Path, case: BenchmarkCase, timeout_seconds: int
) -> list[dict[str, str]]:
    completed = subprocess.run(
        [str(executable), *case.arguments],
        check=True,
        capture_output=True,
        text=True,
        timeout=timeout_seconds,
    )
    reader = csv.DictReader(io.StringIO(completed.stdout))
    if reader.fieldnames != CSV_FIELDS:
        raise ValueError(f"unexpected CSV columns for {case.description}")

    rows = list(reader)
    if len(rows) != TIMED_RUNS * 2:
        raise ValueError(f"unexpected run count for {case.description}")

    for row in rows:
        row["graph_family"] = case.family

    component_counts = {row["components"] for row in rows}
    if len(component_counts) != 1:
        raise ValueError(f"component counts disagree for {case.description}")

    expected_run_numbers = {str(run) for run in range(1, TIMED_RUNS + 1)}
    for algorithm in ["kosaraju", "tarjan"]:
        run_numbers = {
            row["run"] for row in rows if row["algorithm"] == algorithm
        }
        if run_numbers != expected_run_numbers:
            raise ValueError(
                f"missing {algorithm} runs for {case.description}"
            )
    return rows


def run_memory_case(
    executable: Path, case: BenchmarkCase, algorithm: str,
    timeout_seconds: int,
) -> dict[str, str]:
    completed = subprocess.run(
        [str(executable), "--memory", algorithm, *case.arguments],
        check=True,
        capture_output=True,
        text=True,
        timeout=timeout_seconds,
    )
    reader = csv.DictReader(io.StringIO(completed.stdout))
    if reader.fieldnames != MEMORY_FIELDS:
        raise ValueError(f"unexpected memory columns for {case.description}")
    rows = list(reader)
    if len(rows) != 1 or rows[0]["algorithm"] != algorithm:
        raise ValueError(f"unexpected memory row for {case.description}")
    rows[0]["graph_family"] = case.family
    if int(rows[0]["peak_working_set_bytes"]) <= 0:
        raise ValueError(f"invalid memory peak for {case.description}")
    return rows[0]


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Run the Kosaraju and Tarjan benchmark cases."
    )
    parser.add_argument(
        "--executable",
        type=Path,
        default=Path("build/Release/scc_benchmark.exe"),
        help="path to the compiled scc_benchmark program",
    )
    parser.add_argument(
        "--output",
        type=Path,
        help="CSV output path (default: results/timings.csv)",
    )
    parser.add_argument(
        "--memory-output",
        type=Path,
        help="memory CSV path (default: results/memory.csv)",
    )
    parser.add_argument(
        "--timeout",
        type=int,
        default=120,
        help="maximum seconds for each individual benchmark process",
    )
    parser.add_argument(
        "--max-vertices",
        type=int,
        help="skip cases above this vertex count during an initial sweep",
    )
    parser.add_argument(
        "--max-edges",
        type=int,
        help="skip cases above this edge count during an initial sweep",
    )
    parser.add_argument(
        "--quick",
        action="store_true",
        help="run one small case from each graph family",
    )
    return parser.parse_args()


def main() -> int:
    arguments = parse_arguments()
    executable = arguments.executable.resolve()
    if not executable.is_file():
        print(f"Benchmark executable not found: {executable}", file=sys.stderr)
        return 1

    cases = quick_cases() if arguments.quick else full_cases()
    if arguments.max_vertices is not None:
        cases = [
            case for case in cases
            if vertex_count_for(case) <= arguments.max_vertices
        ]
    if arguments.max_edges is not None:
        cases = [
            case for case in cases
            if edge_count_for(case) <= arguments.max_edges
        ]
    if arguments.timeout < 1:
        print("--timeout must be positive", file=sys.stderr)
        return 1
    default_name = "timings-quick.csv" if arguments.quick else "timings.csv"
    output_path = arguments.output or Path("results") / default_name
    memory_name = "memory-quick.csv" if arguments.quick else "memory.csv"
    memory_path = arguments.memory_output or Path("results") / memory_name
    output_path.parent.mkdir(parents=True, exist_ok=True)
    memory_path.parent.mkdir(parents=True, exist_ok=True)

    try:
        with (
            output_path.open("w", newline="", encoding="utf-8") as output,
            memory_path.open("w", newline="", encoding="utf-8") as memory_output,
        ):
            writer = csv.DictWriter(output, fieldnames=CSV_FIELDS)
            memory_writer = csv.DictWriter(
                memory_output, fieldnames=MEMORY_FIELDS
            )
            writer.writeheader()
            memory_writer.writeheader()

            for position, case in enumerate(cases, start=1):
                print(
                    f"[{position}/{len(cases)}] {case.description}",
                    file=sys.stderr,
                    flush=True,
                )
                timing_rows = run_case(
                    executable, case, arguments.timeout
                )
                memory_rows = [
                    run_memory_case(
                        executable, case, algorithm, arguments.timeout
                    )
                    for algorithm in ("kosaraju", "tarjan")
                ]
                expected_components = timing_rows[0]["components"]
                if any(
                    row["components"] != expected_components
                    for row in memory_rows
                ):
                    raise ValueError(
                        f"timing and memory results disagree for {case.description}"
                    )
                writer.writerows(timing_rows)
                memory_writer.writerows(memory_rows)
                output.flush()
                memory_output.flush()
    except KeyboardInterrupt:
        print("\nStopped. Completed rows were kept in the output file.",
              file=sys.stderr)
        return 130
    except (
        OSError, subprocess.CalledProcessError, subprocess.TimeoutExpired,
        ValueError,
    ) as error:
        print(f"Benchmark stopped: {error}", file=sys.stderr)
        return 1

    print(
        f"Saved {len(cases)} cases to {output_path} and {memory_path}",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

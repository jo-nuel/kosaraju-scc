"""Draw benchmark figures from the saved timing and memory CSV files."""

import argparse
import csv
import math
import statistics
from collections import defaultdict
from pathlib import Path
from xml.sax.saxutils import escape


FAMILIES = (
    ("path", "Path"),
    ("cycle", "Cycle"),
    ("sparse_random", "Sparse random"),
    ("dense_random", "Dense random"),
    ("clustered", "Clustered"),
)
ALGORITHMS = ("kosaraju", "tarjan")
COLORS = {"kosaraju": "#2457a7", "tarjan": "#c65d20"}
FIELDS = ("graph_family", "vertices", "edges", "seed", "algorithm")


def read_csv(path, expected_columns):
    with path.open(newline="", encoding="utf-8") as source:
        reader = csv.DictReader(source)
        if reader.fieldnames != expected_columns:
            raise ValueError(f"Unexpected columns in {path}")
        return list(reader)


def case_key(row):
    return tuple(row[field] for field in FIELDS)


def load_results(timing_path, memory_path):
    timings = read_csv(
        timing_path,
        [*FIELDS, "run", "nanoseconds", "components"],
    )
    memory = read_csv(
        memory_path,
        [*FIELDS, "peak_working_set_bytes", "components"],
    )
    by_case = defaultdict(list)
    run_numbers = defaultdict(set)
    components = {}
    for row in timings:
        key = case_key(row)
        run = int(row["run"])
        if run in run_numbers[key]:
            raise ValueError(f"Repeated run number for {key}")
        run_numbers[key].add(run)
        elapsed = int(row["nanoseconds"])
        if elapsed < 0:
            raise ValueError(f"Negative time for {key}")
        if key in components and components[key] != row["components"]:
            raise ValueError(f"Component counts disagree for {key}")
        components[key] = row["components"]
        by_case[key].append(elapsed)

    peaks = {}
    for row in memory:
        key = case_key(row)
        if key in peaks:
            raise ValueError(f"Repeated memory row for {key}")
        peak = int(row["peak_working_set_bytes"])
        if peak <= 0:
            raise ValueError(f"Invalid memory peak for {key}")
        if key in components and components[key] != row["components"]:
            raise ValueError(f"Timing and memory components disagree for {key}")
        peaks[key] = peak

    if not by_case or by_case.keys() != peaks.keys():
        raise ValueError("Timing and memory cases do not match")

    results = defaultdict(dict)
    for key, times in by_case.items():
        family, vertices, edges, seed, algorithm = key
        if family not in {name for name, _ in FAMILIES}:
            raise ValueError(f"Unknown graph family: {family}")
        if algorithm not in ALGORITHMS:
            raise ValueError(f"Unknown algorithm: {algorithm}")
        if len(times) != 7:
            raise ValueError(f"Expected seven timed runs for {key}")
        case = (family, int(vertices), int(edges), seed)
        results[case][algorithm] = (
            statistics.median(times) / 1_000_000,
            peaks[key] / (1024 * 1024),
        )

    for case, algorithms in results.items():
        if set(algorithms) != set(ALGORITHMS):
            raise ValueError(f"Missing algorithm for {case}")
    return results


def svg_text(x, y, value, size=14, anchor="start", color="#263238"):
    return (
        f'<text x="{x:.1f}" y="{y:.1f}" font-size="{size}" '
        f'text-anchor="{anchor}" fill="{color}">{escape(str(value))}</text>'
    )


def draw_panel(parts, x, y, title, points, metric):
    left, right = x + 78, x + 475
    top, bottom = y + 43, y + 230
    parts.append(svg_text(x + 18, y + 24, title, 18))
    if not points:
        parts.append(svg_text(left, top + 75, "No measured cases"))
        return

    vertices = sorted({case[1] for case, _ in points})
    low_x, high_x = math.log10(vertices[0]), math.log10(vertices[-1])
    highest = max(value[metric] for _, pair in points for value in pair.values())
    top_value = highest * 1.1 if highest else 1

    def x_position(vertex):
        if low_x == high_x:
            return (left + right) / 2
        return left + (math.log10(vertex) - low_x) / (high_x - low_x) * (right - left)

    def y_position(value):
        return bottom - value / top_value * (bottom - top)

    for fraction in (0, 0.5, 1):
        y_tick = bottom - fraction * (bottom - top)
        parts.append(
            f'<line x1="{left}" y1="{y_tick:.1f}" x2="{right}" '
            f'y2="{y_tick:.1f}" stroke="#e2e6ea"/>'
        )
        parts.append(
            svg_text(left - 9, y_tick + 5, f"{fraction * top_value:.1f}",
                     12, "end")
        )
    parts.append(
        f'<line x1="{left}" y1="{top}" x2="{left}" y2="{bottom}" '
        'stroke="#64717d"/>'
    )
    parts.append(
        f'<line x1="{left}" y1="{bottom}" x2="{right}" y2="{bottom}" '
        'stroke="#64717d"/>'
    )

    tick_vertices = sorted({vertices[0], vertices[len(vertices) // 2], vertices[-1]})
    for vertex in tick_vertices:
        tick_x = x_position(vertex)
        parts.append(
            f'<line x1="{tick_x:.1f}" y1="{bottom}" x2="{tick_x:.1f}" '
            f'y2="{bottom + 5}" stroke="#64717d"/>'
        )
        parts.append(svg_text(tick_x, bottom + 21, f"{vertex:,}", 12, "middle"))

    for algorithm in ALGORITHMS:
        series = sorted(
            (case[1], pair[algorithm][metric]) for case, pair in points
        )
        coordinates = [
            (x_position(vertex), y_position(value)) for vertex, value in series
        ]
        path = " ".join(f"{px:.1f},{py:.1f}" for px, py in coordinates)
        color = COLORS[algorithm]
        parts.append(
            f'<polyline points="{path}" fill="none" stroke="{color}" '
            'stroke-width="2.5" stroke-linejoin="round"/>'
        )
        for px, py in coordinates:
            parts.append(
                f'<circle cx="{px:.1f}" cy="{py:.1f}" r="4" fill="{color}"/>'
            )
    parts.append(svg_text((left + right) / 2, bottom + 47, "Vertices", 13,
                          "middle"))


def write_figure(path, results, metric):
    title = "Median running time" if metric == 0 else "Peak process working set"
    unit = "Time (ms)" if metric == 0 else "Memory (MiB)"
    description = (
        "Median of seven timed runs for each algorithm and graph."
        if metric == 0 else
        "One fresh process per algorithm and graph. Includes graph storage."
    )
    parts = [
        '<svg xmlns="http://www.w3.org/2000/svg" width="1100" '
        'height="1060" viewBox="0 0 1100 1060" '
        'font-family="Arial, sans-serif">',
        '<rect width="1100" height="1060" fill="white"/>',
        svg_text(55, 43, title, 26),
        svg_text(55, 70, description, 14),
        svg_text(55, 99, unit, 14),
    ]
    for index, algorithm in enumerate(ALGORITHMS):
        legend_x = 715 + 155 * index
        parts.append(
            f'<line x1="{legend_x}" y1="94" x2="{legend_x + 25}" '
            f'y2="94" stroke="{COLORS[algorithm]}" stroke-width="3"/>'
        )
        parts.append(svg_text(legend_x + 31, 99, algorithm.title(), 14))

    for index, (family, label) in enumerate(FAMILIES):
        x = 45 + (index % 2) * 535
        y = 125 + (index // 2) * 305
        family_points = sorted(
            ((case, pair) for case, pair in results.items()
             if case[0] == family),
            key=lambda entry: entry[0][1],
        )
        draw_panel(parts, x, y, label, family_points, metric)
    parts.append("</svg>")
    path.write_text("\n".join(parts) + "\n", encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--timings", type=Path,
                        default=Path("results/timings-bounded.csv"))
    parser.add_argument("--memory", type=Path,
                        default=Path("results/memory-bounded.csv"))
    parser.add_argument("--output-dir", type=Path, default=Path("results"))
    args = parser.parse_args()
    results = load_results(args.timings, args.memory)
    args.output_dir.mkdir(parents=True, exist_ok=True)
    write_figure(args.output_dir / "running-time.svg", results, 0)
    write_figure(args.output_dir / "peak-memory.svg", results, 1)
    print(f"Plotted {len(results)} graph cases in {args.output_dir}")


if __name__ == "__main__":
    main()

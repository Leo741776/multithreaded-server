"""
Stress test for the server.

Usage:
    python test/stress_test.py
    python test/stress_test.py --waves 20 --requests 100
    python test/stress_test.py --url http://127.0.0.1:8080/

Defaults to 127.0.0.1 instead of localhost: on Windows, localhost can
resolve to IPv6 first and add a ~2 s fallback delay to every request.

Uses only the Python standard library (no pip installs needed).
"""

import argparse
import statistics
import time
import urllib.error
import urllib.request
from collections import Counter
from concurrent.futures import ThreadPoolExecutor


def send_request(url: str, timeout: float) -> tuple[str, float]:
    """Send one GET request. Returns (result label, latency in ms)."""
    start = time.perf_counter()
    try:
        with urllib.request.urlopen(url, timeout=timeout) as response:
            status = response.status
            response.read()
        label = "SUCCESS" if status == 200 else f"BAD STATUS: {status}"
    except urllib.error.HTTPError as e:
        label = f"BAD STATUS: {e.code}"
    except Exception as e:
        label = f"FAILED: {type(e).__name__}"
    latency_ms = (time.perf_counter() - start) * 1000
    return label, latency_ms


def percentile(sorted_values: list[float], pct: float) -> float:
    """Nearest-rank percentile of an already sorted list."""
    index = max(0, min(len(sorted_values) - 1, round(pct / 100 * len(sorted_values)) - 1))
    return sorted_values[index]


def print_summary(results: list[tuple[str, float]], elapsed_s: float, indent: str = "  ") -> None:
    counts = Counter(label for label, _ in results)
    latencies = sorted(latency for _, latency in results)

    for label, count in counts.most_common():
        print(f"{indent}{count:>5}  {label}")

    print(
        f"{indent}latency (ms): "
        f"avg {statistics.mean(latencies):.1f} | "
        f"p95 {percentile(latencies, 95):.1f} | "
        f"max {latencies[-1]:.1f}"
    )
    print(f"{indent}throughput: {len(results) / elapsed_s:.0f} req/s")


def main() -> None:
    parser = argparse.ArgumentParser(description="Stress test the multithreaded HTTP server.")
    parser.add_argument("--waves", type=int, default=10, help="number of request waves")
    parser.add_argument("--requests", type=int, default=50, help="concurrent requests per wave")
    parser.add_argument("--url", default="http://127.0.0.1:8080/", help="target URL")
    parser.add_argument("--timeout", type=float, default=10.0, help="per-request timeout (s)")
    args = parser.parse_args()

    total = args.waves * args.requests
    print(f"Stress testing {args.url}")
    print(f"{args.waves} waves x {args.requests} requests = {total} total requests\n")

    all_results: list[tuple[str, float]] = []
    overall_start = time.perf_counter()

    for wave in range(1, args.waves + 1):
        print(f"Wave {wave}")
        wave_start = time.perf_counter()

        # One worker per request so the whole wave hits the server at once.
        with ThreadPoolExecutor(max_workers=args.requests) as pool:
            futures = [pool.submit(send_request, args.url, args.timeout) for _ in range(args.requests)]
            wave_results = [f.result() for f in futures]

        print_summary(wave_results, time.perf_counter() - wave_start)
        print()
        all_results.extend(wave_results)

    overall_elapsed = time.perf_counter() - overall_start
    successes = sum(1 for label, _ in all_results if label == "SUCCESS")

    print("=" * 40)
    print("Overall results")
    print(f"  {successes}/{total} succeeded ({successes / total * 100:.1f}%)")
    print_summary(all_results, overall_elapsed)
    print("\nStress test complete.")


if __name__ == "__main__":
    main()
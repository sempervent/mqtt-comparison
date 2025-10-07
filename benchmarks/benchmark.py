#!/usr/bin/env python3
"""
Benchmark harness for MQTT comparison project.

Runs performance benchmarks across different languages and encodings.
"""

import argparse
import json
import subprocess
import time
from pathlib import Path
from typing import Dict, List, Any
from dataclasses import dataclass, asdict


@dataclass
class BenchmarkResult:
    """Result of a single benchmark run."""
    language: str
    encoding: str
    message_count: int
    duration: float
    messages_per_second: float
    bytes_sent: int


class BenchmarkHarness:
    """Harness for running MQTT benchmarks."""

    def __init__(self, broker: str = "localhost", port: int = 1883):
        """
        Initialize the benchmark harness.

        Args:
            broker: MQTT broker hostname
            port: MQTT broker port
        """
        self.broker = broker
        self.port = port
        self.results: List[BenchmarkResult] = []

    def run_python_benchmark(self, encoding: str, message_count: int) -> BenchmarkResult:
        """
        Run Python benchmark.

        Args:
            encoding: Encoding format ('json' or 'msgpack')
            message_count: Number of messages to send

        Returns:
            BenchmarkResult object
        """
        print(f"\nRunning Python benchmark with {encoding} encoding...")
        
        start_time = time.time()
        
        cmd = [
            "python3",
            "python/src/publisher.py",
            "--broker", self.broker,
            "--port", str(self.port),
            "--encoding", encoding,
            "--count", str(message_count),
            "--interval", "0.01"
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        duration = time.time() - start_time
        messages_per_second = message_count / duration if duration > 0 else 0
        
        # Rough estimate of bytes (will be more accurate with actual measurement)
        avg_message_size = 150 if encoding == "json" else 100
        bytes_sent = message_count * avg_message_size
        
        return BenchmarkResult(
            language="python",
            encoding=encoding,
            message_count=message_count,
            duration=duration,
            messages_per_second=messages_per_second,
            bytes_sent=bytes_sent
        )

    def run_benchmark(self, language: str, encoding: str, message_count: int):
        """
        Run benchmark for specified language and encoding.

        Args:
            language: Programming language
            encoding: Encoding format
            message_count: Number of messages
        """
        if language == "python":
            result = self.run_python_benchmark(encoding, message_count)
            self.results.append(result)
            self.print_result(result)
        else:
            print(f"⚠ Benchmark for {language} not yet implemented")

    def print_result(self, result: BenchmarkResult):
        """Print benchmark result."""
        print(f"  ✓ Duration: {result.duration:.2f}s")
        print(f"  ✓ Messages/sec: {result.messages_per_second:.2f}")
        print(f"  ✓ Bytes sent: {result.bytes_sent}")

    def save_results(self, output_file: str):
        """
        Save benchmark results to JSON file.

        Args:
            output_file: Output file path
        """
        with open(output_file, 'w') as f:
            json.dump([asdict(r) for r in self.results], f, indent=2)
        print(f"\n✓ Results saved to {output_file}")

    def print_summary(self):
        """Print summary of all benchmark results."""
        if not self.results:
            print("\nNo benchmark results to display")
            return

        print("\n" + "="*60)
        print("BENCHMARK SUMMARY")
        print("="*60)
        
        for result in self.results:
            print(f"\n{result.language.upper()} ({result.encoding})")
            print(f"  Messages: {result.message_count}")
            print(f"  Duration: {result.duration:.2f}s")
            print(f"  Throughput: {result.messages_per_second:.2f} msg/s")
            print(f"  Bytes: {result.bytes_sent}")


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(description="MQTT Benchmark Harness")
    parser.add_argument("--broker", default="localhost", help="MQTT broker hostname")
    parser.add_argument("--port", type=int, default=1883, help="MQTT broker port")
    parser.add_argument("--languages", nargs="+", default=["python"],
                        help="Languages to benchmark")
    parser.add_argument("--encodings", nargs="+", default=["json", "msgpack"],
                        help="Encodings to benchmark")
    parser.add_argument("--count", type=int, default=100,
                        help="Number of messages per benchmark")
    parser.add_argument("--output", default="benchmark_results.json",
                        help="Output file for results")

    args = parser.parse_args()

    print("="*60)
    print("MQTT COMPARISON BENCHMARK HARNESS")
    print("="*60)
    print(f"Broker: {args.broker}:{args.port}")
    print(f"Languages: {', '.join(args.languages)}")
    print(f"Encodings: {', '.join(args.encodings)}")
    print(f"Message count: {args.count}")

    harness = BenchmarkHarness(args.broker, args.port)

    try:
        for language in args.languages:
            for encoding in args.encodings:
                harness.run_benchmark(language, encoding, args.count)

        harness.print_summary()
        harness.save_results(args.output)

    except KeyboardInterrupt:
        print("\n\n✗ Benchmark interrupted")
    except Exception as e:
        print(f"\n✗ Error: {e}")


if __name__ == "__main__":
    main()

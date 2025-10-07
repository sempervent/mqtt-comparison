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

    def run_python_benchmark(self, encoding: str, message_count: int, payload_size: str = "small", qos: int = 1) -> BenchmarkResult:
        """
        Run Python benchmark.

        Args:
            encoding: Encoding format ('json' or 'msgpack')
            message_count: Number of messages to send
            payload_size: Payload size variant ('small', 'medium', 'large')
            qos: Quality of Service level

        Returns:
            BenchmarkResult object
        """
        print(f"\nRunning Python benchmark with {encoding} encoding, {payload_size} payload, QoS {qos}...")
        
        start_time = time.time()
        
        cmd = [
            "python3",
            "python/src/publisher.py",
            "--broker", self.broker,
            "--port", str(self.port),
            "--encoding", encoding,
            "--count", str(message_count),
            "--interval", "0.01",
            "--payload", payload_size,
            "--qos", str(qos)
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        duration = time.time() - start_time
        messages_per_second = message_count / duration if duration > 0 else 0
        
        # Estimate bytes based on payload size
        if payload_size == "small":
            avg_message_size = 150 if encoding == "json" else 100
        elif payload_size == "medium":
            avg_message_size = 2000 if encoding == "json" else 1500
        else:  # large
            avg_message_size = 64000 if encoding == "json" else 50000
        
        bytes_sent = message_count * avg_message_size
        
        return BenchmarkResult(
            language="python",
            encoding=encoding,
            message_count=message_count,
            duration=duration,
            messages_per_second=messages_per_second,
            bytes_sent=bytes_sent
        )

    def run_rust_benchmark(self, encoding: str, message_count: int, payload_size: str = "small", qos: int = 1) -> BenchmarkResult:
        """
        Run Rust benchmark.

        Args:
            encoding: Encoding format ('json' or 'msgpack')
            message_count: Number of messages to send
            payload_size: Payload size variant ('small', 'medium', 'large')
            qos: Quality of Service level

        Returns:
            BenchmarkResult object
        """
        print(f"\nRunning Rust benchmark with {encoding} encoding, {payload_size} payload, QoS {qos}...")
        
        start_time = time.time()
        
        cmd = [
            "cargo", "run", "--bin", "publisher", "--release", "--",
            "--broker", self.broker,
            "--port", str(self.port),
            "--encoding", encoding,
            "--count", str(message_count),
            "--interval", "0.01",
            "--payload", payload_size,
            "--qos", str(qos)
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True, cwd="rust")
        
        duration = time.time() - start_time
        messages_per_second = message_count / duration if duration > 0 else 0
        
        # Estimate bytes based on payload size
        if payload_size == "small":
            avg_message_size = 150 if encoding == "json" else 100
        elif payload_size == "medium":
            avg_message_size = 2000 if encoding == "json" else 1500
        else:  # large
            avg_message_size = 64000 if encoding == "json" else 50000
        
        bytes_sent = message_count * avg_message_size
        
        return BenchmarkResult(
            language="rust",
            encoding=encoding,
            message_count=message_count,
            duration=duration,
            messages_per_second=messages_per_second,
            bytes_sent=bytes_sent
        )

    def run_c_benchmark(self, encoding: str, message_count: int, payload_size: str = "small", qos: int = 1) -> BenchmarkResult:
        """
        Run C benchmark.

        Args:
            encoding: Encoding format ('json' or 'msgpack')
            message_count: Number of messages to send
            payload_size: Payload size variant ('small', 'medium', 'large')
            qos: Quality of Service level

        Returns:
            BenchmarkResult object
        """
        print(f"\nRunning C benchmark with {encoding} encoding, {payload_size} payload, QoS {qos}...")
        
        start_time = time.time()
        
        cmd = [
            "./c/bin/publisher",
            "--broker", self.broker,
            "--port", str(self.port),
            "--count", str(message_count),
            "--interval", "0.01",
            "--payload", payload_size,
            "--qos", str(qos)
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        duration = time.time() - start_time
        messages_per_second = message_count / duration if duration > 0 else 0
        
        # Estimate bytes based on payload size
        if payload_size == "small":
            avg_message_size = 150 if encoding == "json" else 100
        elif payload_size == "medium":
            avg_message_size = 2000 if encoding == "json" else 1500
        else:  # large
            avg_message_size = 64000 if encoding == "json" else 50000
        
        bytes_sent = message_count * avg_message_size
        
        return BenchmarkResult(
            language="c",
            encoding=encoding,
            message_count=message_count,
            duration=duration,
            messages_per_second=messages_per_second,
            bytes_sent=bytes_sent
        )

    def run_cpp_benchmark(self, encoding: str, message_count: int, payload_size: str = "small", qos: int = 1) -> BenchmarkResult:
        """
        Run C++ benchmark.

        Args:
            encoding: Encoding format ('json' or 'msgpack')
            message_count: Number of messages to send
            payload_size: Payload size variant ('small', 'medium', 'large')
            qos: Quality of Service level

        Returns:
            BenchmarkResult object
        """
        print(f"\nRunning C++ benchmark with {encoding} encoding, {payload_size} payload, QoS {qos}...")
        
        start_time = time.time()
        
        cmd = [
            "./cpp/bin/publisher",
            "--broker", self.broker,
            "--port", str(self.port),
            "--count", str(message_count),
            "--interval", "0.01",
            "--payload", payload_size,
            "--qos", str(qos)
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        duration = time.time() - start_time
        messages_per_second = message_count / duration if duration > 0 else 0
        
        # Estimate bytes based on payload size
        if payload_size == "small":
            avg_message_size = 150 if encoding == "json" else 100
        elif payload_size == "medium":
            avg_message_size = 2000 if encoding == "json" else 1500
        else:  # large
            avg_message_size = 64000 if encoding == "json" else 50000
        
        bytes_sent = message_count * avg_message_size
        
        return BenchmarkResult(
            language="cpp",
            encoding=encoding,
            message_count=message_count,
            duration=duration,
            messages_per_second=messages_per_second,
            bytes_sent=bytes_sent
        )

    def run_julia_benchmark(self, encoding: str, message_count: int, payload_size: str = "small", qos: int = 1) -> BenchmarkResult:
        """
        Run Julia benchmark.

        Args:
            encoding: Encoding format ('json' or 'msgpack')
            message_count: Number of messages to send
            payload_size: Payload size variant ('small', 'medium', 'large')
            qos: Quality of Service level

        Returns:
            BenchmarkResult object
        """
        print(f"\nRunning Julia benchmark with {encoding} encoding, {payload_size} payload, QoS {qos}...")
        
        start_time = time.time()
        
        cmd = [
            "julia", "--project=.", "src/publisher.jl",
            "--broker", self.broker,
            "--port", str(self.port),
            "--count", str(message_count),
            "--interval", "0.01",
            "--payload", payload_size,
            "--qos", str(qos)
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True, cwd="julia")
        
        duration = time.time() - start_time
        messages_per_second = message_count / duration if duration > 0 else 0
        
        # Estimate bytes based on payload size
        if payload_size == "small":
            avg_message_size = 150 if encoding == "json" else 100
        elif payload_size == "medium":
            avg_message_size = 2000 if encoding == "json" else 1500
        else:  # large
            avg_message_size = 64000 if encoding == "json" else 50000
        
        bytes_sent = message_count * avg_message_size
        
        return BenchmarkResult(
            language="julia",
            encoding=encoding,
            message_count=message_count,
            duration=duration,
            messages_per_second=messages_per_second,
            bytes_sent=bytes_sent
        )

    def run_r_benchmark(self, encoding: str, message_count: int, payload_size: str = "small", qos: int = 1) -> BenchmarkResult:
        """
        Run R benchmark.

        Args:
            encoding: Encoding format ('json' or 'msgpack')
            message_count: Number of messages to send
            payload_size: Payload size variant ('small', 'medium', 'large')
            qos: Quality of Service level

        Returns:
            BenchmarkResult object
        """
        print(f"\nRunning R benchmark with {encoding} encoding, {payload_size} payload, QoS {qos}...")
        
        start_time = time.time()
        
        cmd = [
            "Rscript", "publisher.R",
            "--broker", self.broker,
            "--port", str(self.port),
            "--count", str(message_count),
            "--interval", "0.01",
            "--payload", payload_size,
            "--qos", str(qos)
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True, cwd="r")
        
        duration = time.time() - start_time
        messages_per_second = message_count / duration if duration > 0 else 0
        
        # Estimate bytes based on payload size
        if payload_size == "small":
            avg_message_size = 150 if encoding == "json" else 100
        elif payload_size == "medium":
            avg_message_size = 2000 if encoding == "json" else 1500
        else:  # large
            avg_message_size = 64000 if encoding == "json" else 50000
        
        bytes_sent = message_count * avg_message_size
        
        return BenchmarkResult(
            language="r",
            encoding=encoding,
            message_count=message_count,
            duration=duration,
            messages_per_second=messages_per_second,
            bytes_sent=bytes_sent
        )

    def run_csharp_benchmark(self, encoding: str, message_count: int, payload_size: str = "small", qos: int = 1) -> BenchmarkResult:
        """
        Run C# benchmark.

        Args:
            encoding: Encoding format ('json' or 'msgpack')
            message_count: Number of messages to send
            payload_size: Payload size variant ('small', 'medium', 'large')
            qos: Quality of Service level

        Returns:
            BenchmarkResult object
        """
        print(f"\nRunning C# benchmark with {encoding} encoding, {payload_size} payload, QoS {qos}...")
        
        start_time = time.time()
        
        cmd = [
            "dotnet", "run", "--project", "MQTTComparison.csproj", "--",
            "--publisher",
            "--broker", self.broker,
            "--port", str(self.port),
            "--count", str(message_count),
            "--interval", "0.01",
            "--payload", payload_size,
            "--qos", str(qos)
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True, cwd="csharp")
        
        duration = time.time() - start_time
        messages_per_second = message_count / duration if duration > 0 else 0
        
        # Estimate bytes based on payload size
        if payload_size == "small":
            avg_message_size = 150 if encoding == "json" else 100
        elif payload_size == "medium":
            avg_message_size = 2000 if encoding == "json" else 1500
        else:  # large
            avg_message_size = 64000 if encoding == "json" else 50000
        
        bytes_sent = message_count * avg_message_size
        
        return BenchmarkResult(
            language="csharp",
            encoding=encoding,
            message_count=message_count,
            duration=duration,
            messages_per_second=messages_per_second,
            bytes_sent=bytes_sent
        )

    def run_java_benchmark(self, encoding: str, message_count: int, payload_size: str = "small", qos: int = 1) -> BenchmarkResult:
        """
        Run Java benchmark.

        Args:
            encoding: Encoding format ('json', 'msgpack', 'cbor', 'protobuf')
            message_count: Number of messages to send
            payload_size: Payload size variant ('small', 'medium', 'large')
            qos: Quality of Service level

        Returns:
            BenchmarkResult object
        """
        print(f"\nRunning Java benchmark with {encoding} encoding, {payload_size} payload, QoS {qos}...")
        
        start_time = time.time()
        
        cmd = [
            "java", "-jar", "target/mqtt-java-1.0.0.jar", "publisher",
            "--broker", self.broker,
            "--port", str(self.port),
            "--count", str(message_count),
            "--interval", "0.01",
            "--payload", payload_size,
            "--qos", str(qos),
            "--encoding", encoding
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True, cwd="java")
        
        duration = time.time() - start_time
        messages_per_second = message_count / duration if duration > 0 else 0
        
        # Estimate bytes based on payload size
        if payload_size == "small":
            avg_message_size = 150 if encoding == "json" else 100
        elif payload_size == "medium":
            avg_message_size = 2000 if encoding == "json" else 1500
        else:  # large
            avg_message_size = 64000 if encoding == "json" else 50000
        
        bytes_sent = message_count * avg_message_size
        
        return BenchmarkResult(
            language="java",
            encoding=encoding,
            message_count=message_count,
            duration=duration,
            messages_per_second=messages_per_second,
            bytes_sent=bytes_sent
        )

    def run_benchmark(self, language: str, encoding: str, message_count: int, payload_size: str = "small", qos: int = 1):
        """
        Run benchmark for specified language and encoding.

        Args:
            language: Programming language
            encoding: Encoding format
            message_count: Number of messages
            payload_size: Payload size variant
            qos: Quality of Service level
        """
        if language == "python":
            result = self.run_python_benchmark(encoding, message_count, payload_size, qos)
            self.results.append(result)
            self.print_result(result)
        elif language == "rust":
            result = self.run_rust_benchmark(encoding, message_count, payload_size, qos)
            self.results.append(result)
            self.print_result(result)
        elif language == "c":
            result = self.run_c_benchmark(encoding, message_count, payload_size, qos)
            self.results.append(result)
            self.print_result(result)
        elif language == "cpp":
            result = self.run_cpp_benchmark(encoding, message_count, payload_size, qos)
            self.results.append(result)
            self.print_result(result)
        elif language == "julia":
            result = self.run_julia_benchmark(encoding, message_count, payload_size, qos)
            self.results.append(result)
            self.print_result(result)
        elif language == "r":
            result = self.run_r_benchmark(encoding, message_count, payload_size, qos)
            self.results.append(result)
            self.print_result(result)
        elif language == "csharp":
            result = self.run_csharp_benchmark(encoding, message_count, payload_size, qos)
            self.results.append(result)
            self.print_result(result)
        elif language == "java":
            result = self.run_java_benchmark(encoding, message_count, payload_size, qos)
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
    parser.add_argument("--languages", nargs="+", 
                       choices=["python", "rust", "c", "cpp", "julia", "r", "csharp", "java"],
                       default=["python"],
                       help="Languages to benchmark")
    parser.add_argument("--encodings", nargs="+", default=["json", "msgpack", "cbor", "protobuf"],
                        help="Encodings to benchmark")
    parser.add_argument("--payloads", nargs="+", default=["small"],
                        help="Payload sizes to benchmark")
    parser.add_argument("--qos", nargs="+", type=int, default=[1],
                        help="QoS levels to benchmark")
    parser.add_argument("--count", type=int, default=100,
                        help="Number of messages per benchmark")
    parser.add_argument("--output", default="results/python/benchmark_results.json",
                        help="Output file for results")

    args = parser.parse_args()

    print("="*60)
    print("MQTT COMPARISON BENCHMARK HARNESS")
    print("="*60)
    print(f"Broker: {args.broker}:{args.port}")
    print(f"Languages: {', '.join(args.languages)}")
    print(f"Encodings: {', '.join(args.encodings)}")
    print(f"Payloads: {', '.join(args.payloads)}")
    print(f"QoS: {', '.join(map(str, args.qos))}")
    print(f"Message count: {args.count}")

    harness = BenchmarkHarness(args.broker, args.port)

    try:
        for language in args.languages:
            for encoding in args.encodings:
                for payload in args.payloads:
                    for qos in args.qos:
                        harness.run_benchmark(language, encoding, args.count, payload, qos)

        harness.print_summary()
        harness.save_results(args.output)

    except KeyboardInterrupt:
        print("\n\n✗ Benchmark interrupted")
    except Exception as e:
        print(f"\n✗ Error: {e}")


if __name__ == "__main__":
    main()

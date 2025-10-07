#!/usr/bin/env python3
"""
Run all benchmarks and generate comprehensive results.

This script orchestrates running benchmarks across all languages and encodings,
then generates a unified report.
"""

import subprocess
import time
import json
from pathlib import Path
import argparse


def run_benchmark_suite():
    """Run the complete benchmark suite."""
    print("="*60)
    print("MQTT COMPARISON - COMPLETE BENCHMARK SUITE")
    print("="*60)
    
    # Define benchmark configurations
    configurations = [
        # Python benchmarks
        {
            "language": "python",
            "encodings": ["json", "msgpack", "cbor", "protobuf"],
            "payloads": ["small", "medium", "large"],
            "qos": [0, 1, 2],
            "count": 100
        },
        # Rust benchmarks
        {
            "language": "rust",
            "encodings": ["json", "msgpack"],
            "payloads": ["small", "medium", "large"],
            "qos": [0, 1, 2],
            "count": 100
        },
        # C benchmarks
        {
            "language": "c",
            "encodings": ["json"],
            "payloads": ["small", "medium", "large"],
            "qos": [0, 1, 2],
            "count": 100
        },
        # C++ benchmarks
        {
            "language": "cpp",
            "encodings": ["json"],
            "payloads": ["small", "medium", "large"],
            "qos": [0, 1, 2],
            "count": 100
        },
        # Julia benchmarks
        {
            "language": "julia",
            "encodings": ["json"],
            "payloads": ["small", "medium", "large"],
            "qos": [0, 1, 2],
            "count": 100
        },
        # R benchmarks
        {
            "language": "r",
            "encodings": ["json"],
            "payloads": ["small", "medium", "large"],
            "qos": [0, 1, 2],
            "count": 100
        },
        # C# benchmarks
        {
            "language": "csharp",
            "encodings": ["json"],
            "payloads": ["small", "medium", "large"],
            "qos": [0, 1, 2],
            "count": 100
        }
    ]
    
    results = []
    
    for config in configurations:
        print(f"\nRunning {config['language']} benchmarks...")
        
        try:
            # Run benchmark for this language
            cmd = [
                "python3", "benchmark.py",
                "--languages", config["language"],
                "--encodings"] + config["encodings"] + [
                "--payloads"] + config["payloads"] + [
                "--qos"] + [str(q) for q in config["qos"]] + [
                "--count", str(config["count"]),
                "--output", f"results/{config['language']}/benchmark_results.json"
            ]
            
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            if result.returncode == 0:
                print(f"✓ {config['language']} benchmarks completed")
                
                # Load results
                results_file = Path(f"results/{config['language']}/benchmark_results.json")
                if results_file.exists():
                    with open(results_file, 'r') as f:
                        lang_results = json.load(f)
                        results.extend(lang_results)
            else:
                print(f"✗ {config['language']} benchmarks failed: {result.stderr}")
                
        except Exception as e:
            print(f"✗ Error running {config['language']} benchmarks: {e}")
    
    # Generate comprehensive report
    print("\nGenerating comprehensive report...")
    
    try:
        # Run results analysis
        analysis_cmd = ["python3", "analyze_results.py", "--output", "docs/benchmark_report.md"]
        subprocess.run(analysis_cmd, check=True)
        print("✓ Comprehensive report generated")
        
    except subprocess.CalledProcessError as e:
        print(f"✗ Error generating report: {e}")
    
    print("\n" + "="*60)
    print("BENCHMARK SUITE COMPLETE")
    print("="*60)
    print(f"Total results: {len(results)}")
    print("Report: docs/benchmark_report.md")


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(description="Run complete MQTT benchmark suite")
    parser.add_argument("--quick", action="store_true", help="Run quick benchmarks (reduced count)")
    
    args = parser.parse_args()
    
    if args.quick:
        print("Running quick benchmark suite...")
        # Modify configurations for quick run
        # This would reduce the number of tests
        pass
    
    run_benchmark_suite()


if __name__ == "__main__":
    main()

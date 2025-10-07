#!/usr/bin/env python3
"""
Results analysis script for MQTT comparison benchmarks.

Analyzes benchmark results and generates comparison tables and charts.
"""

import json
import os
import pandas as pd
from pathlib import Path
from typing import List, Dict, Any
import argparse


class ResultsAnalyzer:
    """Analyzer for benchmark results."""
    
    def __init__(self, results_dir: str = "results"):
        """
        Initialize the results analyzer.
        
        Args:
            results_dir: Directory containing benchmark results
        """
        self.results_dir = Path(results_dir)
        self.results = []
        
    def load_results(self):
        """Load all benchmark results from the results directory."""
        for lang_dir in self.results_dir.iterdir():
            if lang_dir.is_dir():
                results_file = lang_dir / "benchmark_results.json"
                if results_file.exists():
                    with open(results_file, 'r') as f:
                        lang_results = json.load(f)
                        self.results.extend(lang_results)
    
    def create_summary_table(self) -> pd.DataFrame:
        """Create a summary table of all results."""
        if not self.results:
            return pd.DataFrame()
        
        df = pd.DataFrame(self.results)
        
        # Create summary statistics
        summary = df.groupby(['language', 'encoding', 'payload_size']).agg({
            'duration': ['mean', 'std'],
            'messages_per_second': ['mean', 'std'],
            'bytes_sent': ['mean', 'std']
        }).round(2)
        
        return summary
    
    def create_performance_table(self) -> pd.DataFrame:
        """Create a performance comparison table."""
        if not self.results:
            return pd.DataFrame()
        
        df = pd.DataFrame(self.results)
        
        # Group by language and encoding, calculate averages
        performance = df.groupby(['language', 'encoding']).agg({
            'messages_per_second': 'mean',
            'duration': 'mean',
            'bytes_sent': 'mean'
        }).round(2)
        
        # Sort by messages per second
        performance = performance.sort_values('messages_per_second', ascending=False)
        
        return performance
    
    def create_encoding_comparison(self) -> pd.DataFrame:
        """Create encoding performance comparison."""
        if not self.results:
            return pd.DataFrame()
        
        df = pd.DataFrame(self.results)
        
        # Group by encoding, calculate averages
        encoding_perf = df.groupby('encoding').agg({
            'messages_per_second': 'mean',
            'duration': 'mean',
            'bytes_sent': 'mean'
        }).round(2)
        
        # Sort by messages per second
        encoding_perf = encoding_perf.sort_values('messages_per_second', ascending=False)
        
        return encoding_perf
    
    def create_language_comparison(self) -> pd.DataFrame:
        """Create language performance comparison."""
        if not self.results:
            return pd.DataFrame()
        
        df = pd.DataFrame(self.results)
        
        # Group by language, calculate averages
        lang_perf = df.groupby('language').agg({
            'messages_per_second': 'mean',
            'duration': 'mean',
            'bytes_sent': 'mean'
        }).round(2)
        
        # Sort by messages per second
        lang_perf = lang_perf.sort_values('messages_per_second', ascending=False)
        
        return lang_perf
    
    def generate_report(self, output_file: str = "docs/benchmark_report.md"):
        """Generate a comprehensive benchmark report."""
        self.load_results()
        
        if not self.results:
            print("No benchmark results found.")
            return
        
        # Create output directory
        os.makedirs(os.path.dirname(output_file), exist_ok=True)
        
        with open(output_file, 'w') as f:
            f.write("# MQTT Comparison Benchmark Report\n\n")
            f.write("## Overview\n\n")
            f.write(f"Total benchmark runs: {len(self.results)}\n\n")
            
            # Performance table
            f.write("## Performance Comparison\n\n")
            perf_table = self.create_performance_table()
            f.write("### By Language and Encoding\n\n")
            f.write(perf_table.to_markdown())
            f.write("\n\n")
            
            # Encoding comparison
            f.write("### By Encoding\n\n")
            encoding_table = self.create_encoding_comparison()
            f.write(encoding_table.to_markdown())
            f.write("\n\n")
            
            # Language comparison
            f.write("### By Language\n\n")
            lang_table = self.create_language_comparison()
            f.write(lang_table.to_markdown())
            f.write("\n\n")
            
            # Summary statistics
            f.write("## Summary Statistics\n\n")
            summary_table = self.create_summary_table()
            f.write(summary_table.to_markdown())
            f.write("\n\n")
            
            # Raw results
            f.write("## Raw Results\n\n")
            f.write("```json\n")
            f.write(json.dumps(self.results, indent=2))
            f.write("\n```\n")
        
        print(f"Benchmark report generated: {output_file}")


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(description="Analyze MQTT benchmark results")
    parser.add_argument("--results-dir", default="results", help="Results directory")
    parser.add_argument("--output", default="docs/benchmark_report.md", help="Output file")
    
    args = parser.parse_args()
    
    analyzer = ResultsAnalyzer(args.results_dir)
    analyzer.generate_report(args.output)


if __name__ == "__main__":
    main()

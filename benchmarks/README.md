# Benchmark Harness

Python-based benchmark harness for comparing MQTT implementations across languages and encodings.

## Features

- Automated benchmark execution
- Performance metrics collection (throughput, latency)
- JSON output for analysis
- Support for multiple languages and encodings

## Usage

```bash
# Run benchmarks for Python with both JSON and MessagePack
python3 benchmark.py --languages python --encodings json msgpack --count 100

# Run with custom broker
python3 benchmark.py --broker mqtt-broker.example.com --port 1883

# Save results to custom file
python3 benchmark.py --output results.json
```

## Output

Results are saved in JSON format:

```json
[
  {
    "language": "python",
    "encoding": "json",
    "message_count": 100,
    "duration": 1.23,
    "messages_per_second": 81.3,
    "bytes_sent": 15000
  }
]
```

## Adding Language Support

To add benchmark support for a new language:

1. Implement the benchmark method in `BenchmarkHarness` class
2. Add the language to the `--languages` options
3. Ensure the language implementation follows the standard interface

Example:

```python
def run_rust_benchmark(self, encoding: str, message_count: int) -> BenchmarkResult:
    # Implementation here
    pass
```

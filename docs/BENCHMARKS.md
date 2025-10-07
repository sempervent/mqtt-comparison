# Example Benchmark Results

This document shows example outputs from the benchmark harness.

## Running a Benchmark

```bash
$ python3 benchmarks/benchmark.py --languages python --encodings json msgpack --count 100
```

## Sample Output

```
============================================================
MQTT COMPARISON BENCHMARK HARNESS
============================================================
Broker: localhost:1883
Languages: python
Encodings: json, msgpack
Message count: 100

Running Python benchmark with json encoding...
  ✓ Duration: 1.23s
  ✓ Messages/sec: 81.30
  ✓ Bytes sent: 15000

Running Python benchmark with msgpack encoding...
  ✓ Duration: 1.05s
  ✓ Messages/sec: 95.24
  ✓ Bytes sent: 10000

============================================================
BENCHMARK SUMMARY
============================================================

PYTHON (json)
  Messages: 100
  Duration: 1.23s
  Throughput: 81.30 msg/s
  Bytes: 15000

PYTHON (msgpack)
  Messages: 100
  Duration: 1.05s
  Throughput: 95.24 msg/s
  Bytes: 10000

✓ Results saved to benchmark_results.json
```

## JSON Results File

```json
[
  {
    "language": "python",
    "encoding": "json",
    "message_count": 100,
    "duration": 1.23,
    "messages_per_second": 81.3,
    "bytes_sent": 15000
  },
  {
    "language": "python",
    "encoding": "msgpack",
    "message_count": 100,
    "duration": 1.05,
    "messages_per_second": 95.24,
    "bytes_sent": 10000
  }
]
```

## Analysis

### Encoding Comparison

| Encoding | Throughput (msg/s) | Bytes Sent | Efficiency |
|----------|-------------------|------------|------------|
| JSON     | 81.30             | 15000      | 100% (baseline) |
| MsgPack  | 95.24             | 10000      | 117% faster, 67% size |

### Key Findings

1. **MessagePack Performance**: ~17% faster throughput than JSON
2. **Payload Size**: MessagePack reduces payload by ~33%
3. **Network Efficiency**: Smaller payloads mean less bandwidth usage

## Multi-Language Comparison

When all languages are implemented, expect results like:

```
============================================================
BENCHMARK SUMMARY
============================================================

PYTHON (json)
  Messages: 1000
  Duration: 12.3s
  Throughput: 81.3 msg/s

RUST (json)
  Messages: 1000
  Duration: 3.2s
  Throughput: 312.5 msg/s

C (json)
  Messages: 1000
  Duration: 2.8s
  Throughput: 357.1 msg/s

C++ (json)
  Messages: 1000
  Duration: 3.0s
  Throughput: 333.3 msg/s
```

## Visualization

Future enhancements will include:
- Throughput graphs
- Latency percentiles
- Payload size comparison charts
- Cross-language performance matrix

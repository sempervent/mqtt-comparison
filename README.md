# MQTT Comparison

[![CI Checks](https://github.com/sempervent/mqtt-comparison/actions/workflows/ci.yml/badge.svg)](https://github.com/sempervent/mqtt-comparison/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A comprehensive comparison of MQTT publish/subscribe implementations across multiple programming languages and serialization formats.

## Overview

This project compares MQTT pub/sub performance across:

**Languages**: Python, Java, Rust, C, C++, Julia, R, C#

**Encodings**: JSON, MessagePack, CBOR, Protocol Buffers

**Infrastructure**: Docker Compose Mosquitto broker, shared schemas, unified benchmark harness

## Quick Start

### Prerequisites

- Docker 20.10+ and Docker Compose 2.0+
- Python 3.11+ (for local development)
- Language-specific toolchains (for local development)

### Docker Setup (Recommended)

```bash
# Start MQTT broker
docker compose --profile broker up -d

# Test a specific language
./scripts/run-language-test.sh python json small 1 10

# Run all tests
./scripts/run-all-tests.sh

# Run comprehensive benchmark
./scripts/run-benchmark.sh
```

### Local Development

```bash
# Start MQTT broker
docker compose up -d

# Run Python tests locally
python3 python/src/publisher.py --encoding json --payload small --qos 1

# Run benchmark locally
python3 benchmarks/benchmark.py --languages python --encodings json
```

### Smoke Test

Run a quick smoke test to verify the setup:

```bash
./smoke.sh
```

## Project Structure

```
mqtt-comparison/
├── docker-compose.yml          # Mosquitto broker setup
├── mosquitto/                  # Broker configuration
│   └── config/
│       └── mosquitto.conf
├── schemas/                    # Shared message schemas
│   ├── sensor_data.json        # JSON Schema
│   └── sensor_data.proto       # Protocol Buffers
├── python/                     # Python implementations
├── rust/                       # Rust implementations
├── c/                          # C implementations
├── cpp/                        # C++ implementations
├── julia/                      # Julia implementations
├── r/                          # R implementations
├── csharp/                     # C# implementations
├── benchmarks/                 # Unified benchmark harness
├── docs/                       # Documentation
│   └── _summaries/             # Phase completion summaries (FIFO)
├── smoke.sh                    # Quick smoke test
└── summarize_phase.py          # Phase summary generator
```

## Development Phases

The project is developed in phases, tracked via `docs/_summaries/`:

1. **Bootstrap** - Core infrastructure (Docker, schemas, docs)
2. **Python** - Python pub/sub with JSON and MsgPack
3. **Rust** - Rust pub/sub with JSON and MsgPack
4. **C/C++** - C and C++ implementations with JSON and MsgPack
5. **Julia/R/C#** - Julia, R, and C# implementations
6. **CBOR/Proto** - Add CBOR and Protobuf support
7. **Unified Benchmarks** - Complete benchmark harness and analysis

### Creating Phase Summaries

After completing a phase:

```bash
./summarize_phase.py phase_name "Summary of what was accomplished"
```

## Message Schema

All implementations use a common sensor data schema:

```json
{
  "timestamp": 1234567890.123,
  "sensor_id": "sensor_001",
  "temperature": 23.5,
  "humidity": 45.2,
  "pressure": 1013.25
}
```

## Testing

### Docker Testing (Recommended)

```bash
# Test specific language
./scripts/run-language-test.sh python json small 1 10
./scripts/run-language-test.sh java msgpack medium 2 50
./scripts/run-language-test.sh rust cbor large 1 100

# Test all languages
./scripts/run-all-tests.sh

# Run comprehensive benchmark
./scripts/run-benchmark.sh
```

### Local Testing

```bash
# Run CI checks locally
./smoke.sh
docker compose up -d
# Run language-specific tests
docker compose down
```

## Docker Setup

The project includes comprehensive Docker support for all languages and the benchmark harness.

### Quick Docker Commands

```bash
# Start MQTT broker
docker compose --profile broker up -d

# Test specific language
docker compose --profile python up -d

# Run benchmark
docker compose run --rm benchmark --languages python --encodings json

# Clean up
docker compose down --volumes
```

### Docker Profiles

- `broker` - MQTT broker only
- `python`, `java`, `rust`, `c`, `cpp`, `julia`, `r`, `csharp` - Language implementations
- `benchmark` - Benchmark harness
- `all` - All services

For detailed Docker documentation, see [DOCKER.md](DOCKER.md).

## Contributing

1. Follow the phase order when adding implementations
2. Use shared schemas from `schemas/` directory
3. Document phase completion with `summarize_phase.py`
4. Ensure CI checks pass

## License

MIT License - see [LICENSE](LICENSE) for details

# MQTT Comparison

A comprehensive comparison of MQTT publish/subscribe implementations across multiple programming languages and serialization formats.

## Overview

This project compares MQTT pub/sub performance across:

**Languages**: Python, Rust, C, C++, Julia, R, C#

**Encodings**: JSON, MessagePack, CBOR, Protocol Buffers

**Infrastructure**: Docker Compose Mosquitto broker, shared schemas, unified benchmark harness

## Quick Start

### Prerequisites

- Docker and Docker Compose
- Python 3.11+
- Language-specific toolchains (as needed)

### Running the MQTT Broker

```bash
docker compose up -d
```

The Mosquitto broker will be available at `localhost:1883`.

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

Run the CI checks locally:

```bash
./smoke.sh
docker compose up -d
# Run language-specific tests
docker compose down
```

## Contributing

1. Follow the phase order when adding implementations
2. Use shared schemas from `schemas/` directory
3. Document phase completion with `summarize_phase.py`
4. Ensure CI checks pass

## License

MIT License - see [LICENSE](LICENSE) for details

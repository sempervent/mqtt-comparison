# Architecture Overview

## System Design

The MQTT comparison project is designed as a modular, multi-language benchmarking system.

### Components

```
┌─────────────────────────────────────────────────────┐
│                 MQTT Broker (Mosquitto)             │
│                  localhost:1883                      │
└─────────────────────────────────────────────────────┘
                          ▲
                          │
        ┌─────────────────┴─────────────────┐
        │                                   │
   Publishers                          Subscribers
        │                                   │
  ┌─────┴─────┬─────────┬──────────┐       │
  │           │         │          │       │
Python      Rust       C/C++    Julia/R/C# │
  │           │         │          │       │
  └───────────┴─────────┴──────────┴───────┘
                          │
                          ▼
              ┌─────────────────────┐
              │  Benchmark Harness  │
              │     (Python)        │
              └─────────────────────┘
```

### Message Flow

1. **Publisher** creates sensor data
2. **Encoder** serializes data (JSON/MsgPack/CBOR/Protobuf)
3. **MQTT Client** publishes to broker
4. **Broker** routes message to subscribers
5. **Subscriber** receives and decodes message
6. **Benchmark Harness** measures performance

### Shared Schema

All implementations use the same sensor data schema:

```json
{
  "timestamp": float,    // Unix timestamp
  "sensor_id": string,   // Sensor identifier
  "temperature": float,  // Celsius
  "humidity": float,     // Percentage
  "pressure": float      // hPa
}
```

### Encoding Formats

#### JSON
- Human-readable
- Widely supported
- Larger payload size
- Baseline for comparison

#### MessagePack
- Binary format
- Smaller payload
- Faster serialization
- Good language support

#### CBOR
- Binary format
- RFC 8949 standard
- Similar to MessagePack
- Better for IoT

#### Protocol Buffers
- Strongly typed
- Schema required
- Most compact
- Fastest serialization

### Language Implementations

Each language provides:
- Publisher implementation
- Subscriber implementation
- Common CLI interface
- Encoding support

### Benchmark Harness

The Python benchmark harness:
- Orchestrates test runs
- Measures throughput and latency
- Collects metrics
- Generates reports

## Directory Structure

```
mqtt-comparison/
├── schemas/              # Shared message schemas
├── python/              # Python implementation
├── rust/                # Rust implementation
├── c/                   # C implementation
├── cpp/                 # C++ implementation
├── julia/               # Julia implementation
├── r/                   # R implementation
├── csharp/              # C# implementation
├── benchmarks/          # Benchmark harness
├── docs/                # Documentation
│   └── _summaries/      # Phase summaries
└── mosquitto/           # MQTT broker config
```

## Testing Strategy

### Unit Tests
- Individual component testing
- Encoder/decoder validation
- Schema validation

### Integration Tests
- End-to-end message flow
- Multi-language interop
- Encoding compatibility

### Performance Tests
- Throughput benchmarks
- Latency measurements
- Resource usage

## Future Enhancements

- WebSocket support (port 9001)
- TLS/SSL encryption
- QoS level comparison
- Retained message testing
- Last Will and Testament
- Session persistence

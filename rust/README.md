# Rust MQTT Implementation

Rust implementation of MQTT publisher and subscriber with JSON and MessagePack encoding support.

## Features

- **Publisher**: Publishes sensor data with configurable payload sizes and QoS levels
- **Subscriber**: Subscribes to MQTT topics and decodes messages
- **Encodings**: JSON and MessagePack support
- **Payload Sizes**: Small (~128B), Medium (~2KB), Large (~64KB)
- **QoS Levels**: 0, 1, and 2
- **Timing**: Publish and receive latency measurements

## Dependencies

- `rumqttc` - MQTT client library
- `serde` - Serialization framework
- `serde_json` - JSON serialization
- `rmp-serde` - MessagePack serialization
- `clap` - Command-line argument parsing
- `tokio` - Async runtime
- `rand` - Random number generation

## Building

```bash
cargo build --release
```

## Usage

### Publisher

```bash
# Basic usage
cargo run --bin publisher

# With custom options
cargo run --bin publisher -- \
  --broker localhost \
  --port 1883 \
  --encoding json \
  --topic mqtt-demo/all \
  --sensor-id sensor_001 \
  --count 100 \
  --interval 1.0 \
  --payload small \
  --qos 1
```

### Subscriber

```bash
# Basic usage
cargo run --bin subscriber

# With custom options
cargo run --bin subscriber -- \
  --broker localhost \
  --port 1883 \
  --encoding json \
  --topic mqtt-demo/all \
  --qos 1
```

## Command Line Options

### Publisher Options

- `--broker`: MQTT broker hostname (default: localhost)
- `--port`: MQTT broker port (default: 1883)
- `--encoding`: Encoding format - json or msgpack (default: json)
- `--topic`: MQTT topic (default: mqtt-demo/all)
- `--sensor-id`: Sensor identifier (default: sensor_001)
- `--count`: Number of messages to publish (default: 10)
- `--interval`: Interval between messages in seconds (default: 1.0)
- `--payload`: Payload size - small, medium, or large (default: small)
- `--qos`: Quality of Service level - 0, 1, or 2 (default: 1)

### Subscriber Options

- `--broker`: MQTT broker hostname (default: localhost)
- `--port`: MQTT broker port (default: 1883)
- `--encoding`: Encoding format - json or msgpack (default: json)
- `--topic`: MQTT topic (default: mqtt-demo/all)
- `--qos`: Quality of Service level - 0, 1, or 2 (default: 1)

## Message Format

The Rust implementation uses the same sensor data format as other languages:

```rust
struct SensorData {
    timestamp: f64,
    sensor_id: String,
    temperature: f64,
    humidity: f64,
    pressure: f64,
    // Additional fields for medium/large payloads
    location: Option<Location>,
    status: Option<String>,
    battery_level: Option<f64>,
    signal_strength: Option<i32>,
    sensor_readings: Option<Vec<f64>>,
    metadata: Option<Metadata>,
    additional_data: Option<String>,
}
```

## Performance

The Rust implementation provides:

- **High throughput**: Optimized for performance with minimal allocations
- **Low latency**: Efficient serialization and network handling
- **Memory safety**: Rust's ownership system prevents memory leaks
- **Concurrency**: Async/await support for handling multiple connections

## Benchmarking

Use the benchmark harness to test performance:

```bash
# From the project root
python3 benchmarks/benchmark.py --languages rust --encodings json msgpack
```

## Error Handling

The implementation includes comprehensive error handling for:

- Network connectivity issues
- Serialization/deserialization errors
- MQTT protocol errors
- Invalid command-line arguments
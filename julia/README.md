# Julia MQTT Implementation

Julia implementation of MQTT publisher and subscriber with JSON encoding support.

## Features

- **Publisher**: Publishes sensor data with configurable payload sizes and QoS levels
- **Subscriber**: Subscribes to MQTT topics and decodes messages
- **Encoding**: JSON support (MessagePack support can be added)
- **Payload Sizes**: Small (~128B), Medium (~2KB), Large (~64KB)
- **QoS Levels**: 0, 1, and 2
- **Timing**: Publish and receive latency measurements

## Dependencies

- `MQTT.jl` - MQTT client library for Julia
- `JSON3.jl` - High-performance JSON library
- `ArgParse.jl` - Command-line argument parsing
- `Random.jl` - Random number generation

## Installation

```bash
# Install Julia dependencies
julia --project=. -e "using Pkg; Pkg.instantiate()"
```

## Usage

### Publisher

```bash
# Basic usage
julia --project=. src/publisher.jl

# With custom options
julia --project=. src/publisher.jl \
  --broker localhost \
  --port 1883 \
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
julia --project=. src/subscriber.jl

# With custom options
julia --project=. src/subscriber.jl \
  --broker localhost \
  --port 1883 \
  --topic mqtt-demo/all \
  --qos 1
```

## Command Line Options

### Publisher Options

- `--broker`: MQTT broker hostname (default: localhost)
- `--port`: MQTT broker port (default: 1883)
- `--topic`: MQTT topic (default: mqtt-demo/all)
- `--sensor-id`: Sensor identifier (default: sensor_001)
- `--count`: Number of messages to publish (default: 10)
- `--interval`: Interval between messages in seconds (default: 1.0)
- `--payload`: Payload size - small, medium, or large (default: small)
- `--qos`: Quality of Service level - 0, 1, or 2 (default: 1)

### Subscriber Options

- `--broker`: MQTT broker hostname (default: localhost)
- `--port`: MQTT broker port (default: 1883)
- `--topic`: MQTT topic (default: mqtt-demo/all)
- `--qos`: Quality of Service level - 0, 1, or 2 (default: 1)

## Message Format

The Julia implementation uses the same sensor data format as other languages:

```julia
mutable struct SensorData
    timestamp::Float64
    sensor_id::String
    temperature::Float64
    humidity::Float64
    pressure::Float64
    
    # Optional fields for larger payloads
    location::Union{Nothing, NamedTuple}
    status::Union{Nothing, String}
    battery_level::Union{Nothing, Float64}
    signal_strength::Union{Nothing, Int32}
    sensor_readings::Union{Nothing, Vector{Float64}}
    metadata::Union{Nothing, NamedTuple}
    additional_data::Union{Nothing, String}
end
```

## Performance

The Julia implementation provides:

- **High performance**: Julia's JIT compilation for optimal speed
- **Memory efficiency**: Efficient memory management with garbage collection
- **Fast serialization**: High-performance JSON encoding/decoding with JSON3.jl
- **Concurrency**: Built-in support for concurrent operations

## Benchmarking

Use the benchmark harness to test performance:

```bash
# From the project root
python3 benchmarks/benchmark.py --languages julia --encodings json
```

## Error Handling

The implementation includes comprehensive error handling for:

- Network connectivity issues
- JSON parsing errors
- MQTT protocol errors
- Invalid command-line arguments
- Interrupt handling (Ctrl+C)

## Memory Management

The implementation uses Julia's automatic memory management:

- Garbage collection for automatic cleanup
- Efficient memory allocation
- No manual memory management required
- Optimized for performance

## Thread Safety

The MQTT client library handles thread safety internally:

- Callback functions are called from the MQTT client thread
- Shared data access is protected
- Signal handlers are thread-safe
- Async operations are properly synchronized

## Julia Features

The implementation uses modern Julia features:

- Multiple dispatch for flexible function behavior
- Type annotations for performance
- Module system for code organization
- Package management with Project.toml
- High-performance JSON with JSON3.jl
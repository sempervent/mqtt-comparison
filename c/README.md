# C MQTT Implementation

C implementation of MQTT publisher and subscriber with JSON encoding support.

## Features

- **Publisher**: Publishes sensor data with configurable payload sizes and QoS levels
- **Subscriber**: Subscribes to MQTT topics and decodes messages
- **Encoding**: JSON support (MessagePack support can be added)
- **Payload Sizes**: Small (~128B), Medium (~2KB), Large (~64KB)
- **QoS Levels**: 0, 1, and 2
- **Timing**: Publish and receive latency measurements

## Dependencies

- `libpaho-mqtt3c` - Eclipse Paho MQTT C client library
- `libjson-c` - JSON-C library for JSON parsing
- `pthread` - POSIX threads library

## Installation

### Ubuntu/Debian
```bash
sudo apt-get install libpaho-mqtt-dev libjson-c-dev
```

### macOS
```bash
brew install paho-mqtt-c json-c
```

### CentOS/RHEL
```bash
sudo yum install paho-mqtt-c-devel json-c-devel
```

## Building

```bash
# Build all binaries
make

# Build specific binary
make publisher
make subscriber

# Clean build artifacts
make clean

# Show help
make help
```

## Usage

### Publisher

```bash
# Basic usage
./bin/publisher

# With custom options
./bin/publisher \
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
./bin/subscriber

# With custom options
./bin/subscriber \
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

The C implementation uses the same sensor data format as other languages:

```c
typedef struct {
    double timestamp;
    char sensor_id[64];
    double temperature;
    double humidity;
    double pressure;
    
    // Optional fields for larger payloads
    struct {
        double lat;
        double lon;
        double altitude;
    } location;
    
    char status[32];
    double battery_level;
    int32_t signal_strength;
    double *sensor_readings;
    size_t sensor_readings_count;
    
    struct {
        char firmware_version[32];
        char hardware_id[32];
        char calibration_date[32];
        char last_maintenance[32];
    } metadata;
    
    char *additional_data;
    size_t additional_data_size;
} sensor_data_t;
```

## Performance

The C implementation provides:

- **High performance**: Native C code with minimal overhead
- **Low memory usage**: Efficient memory management
- **Fast serialization**: Optimized JSON encoding/decoding
- **Thread safety**: Proper handling of concurrent operations

## Benchmarking

Use the benchmark harness to test performance:

```bash
# From the project root
python3 benchmarks/benchmark.py --languages c --encodings json
```

## Error Handling

The implementation includes comprehensive error handling for:

- Memory allocation failures
- Network connectivity issues
- JSON parsing errors
- MQTT protocol errors
- Invalid command-line arguments

## Memory Management

The implementation uses proper memory management:

- Dynamic allocation for variable-size data
- Proper cleanup in all code paths
- Memory leak prevention
- Bounds checking for string operations

## Thread Safety

The MQTT client library handles thread safety internally, but the application code should be aware of:

- Callback functions are called from the MQTT client thread
- Shared data access should be protected if needed
- Signal handlers should be thread-safe
# C++ MQTT Implementation

C++ implementation of MQTT publisher and subscriber with JSON encoding support.

## Features

- **Publisher**: Publishes sensor data with configurable payload sizes and QoS levels
- **Subscriber**: Subscribes to MQTT topics and decodes messages
- **Encoding**: JSON support (MessagePack support can be added)
- **Payload Sizes**: Small (~128B), Medium (~2KB), Large (~64KB)
- **QoS Levels**: 0, 1, and 2
- **Timing**: Publish and receive latency measurements

## Dependencies

- `libpaho-mqttpp3` - Eclipse Paho MQTT C++ client library
- `CMake` 3.10+ - Build system
- `C++17` - C++ standard

## Installation

### Ubuntu/Debian
```bash
sudo apt-get install libpaho-mqttpp3-dev cmake
```

### macOS
```bash
brew install paho-mqtt-cpp cmake
```

### CentOS/RHEL
```bash
sudo yum install paho-mqtt-cpp-devel cmake
```

## Building

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
make

# Install (optional)
make install
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

The C++ implementation uses the same sensor data format as other languages:

```cpp
class SensorData {
public:
    double timestamp;
    std::string sensor_id;
    double temperature;
    double humidity;
    double pressure;
    
    // Optional fields for larger payloads
    Location location;
    std::string status;
    double battery_level;
    int32_t signal_strength;
    std::vector<double> sensor_readings;
    Metadata metadata;
    std::string additional_data;
};
```

## Performance

The C++ implementation provides:

- **High performance**: Native C++ code with minimal overhead
- **Memory efficiency**: RAII and smart pointers for automatic memory management
- **Fast serialization**: Optimized JSON encoding/decoding
- **Async support**: Non-blocking MQTT operations

## Benchmarking

Use the benchmark harness to test performance:

```bash
# From the project root
python3 benchmarks/benchmark.py --languages cpp --encodings json
```

## Error Handling

The implementation includes comprehensive error handling for:

- Network connectivity issues
- JSON parsing errors
- MQTT protocol errors
- Invalid command-line arguments
- Memory allocation failures

## Memory Management

The implementation uses modern C++ memory management:

- RAII (Resource Acquisition Is Initialization)
- Smart pointers where appropriate
- Automatic cleanup in destructors
- Exception safety

## Thread Safety

The MQTT client library handles thread safety internally:

- Callback functions are called from the MQTT client thread
- Shared data access is protected
- Signal handlers are thread-safe
- Async operations are properly synchronized

## Modern C++ Features

The implementation uses modern C++ features:

- C++17 standard
- RAII for resource management
- STL containers and algorithms
- Lambda expressions for callbacks
- Chrono for timing measurements
- Exception handling
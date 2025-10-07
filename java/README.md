# Java MQTT Implementation

Java implementation of MQTT publisher and subscriber with multiple encoding support.

## Features

- **Publisher**: Publishes sensor data with configurable payload sizes and QoS levels
- **Subscriber**: Subscribes to MQTT topics and decodes messages
- **Encodings**: JSON, MessagePack, CBOR, and Protobuf support
- **Payload Sizes**: Small (~128B), Medium (~2KB), Large (~64KB)
- **QoS Levels**: 0, 1, and 2
- **Timing**: Publish and receive latency measurements

## Dependencies

- `Eclipse Paho MQTT Client` - MQTT client library for Java
- `Jackson` - JSON processing and data binding
- `MessagePack` - MessagePack serialization
- `CBOR` - CBOR serialization
- `Protocol Buffers` - Google's protobuf library
- `Picocli` - Command-line argument parsing

## Installation

```bash
# Install Java 11+ and Maven
# https://adoptium.net/ or https://openjdk.java.net/

# Build the project
mvn clean package

# Run with Maven
mvn exec:java -Dexec.mainClass="com.mqttcomparison.Main" -Dexec.args="publisher"
```

## Usage

### Publisher

```bash
# Basic usage
java -jar target/mqtt-java-1.0.0.jar publisher

# With custom options
java -jar target/mqtt-java-1.0.0.jar publisher \
  --broker localhost \
  --port 1883 \
  --topic mqtt-demo/all \
  --sensor-id sensor_001 \
  --count 100 \
  --interval 1.0 \
  --payload small \
  --qos 1 \
  --encoding json
```

### Subscriber

```bash
# Basic usage
java -jar target/mqtt-java-1.0.0.jar subscriber

# With custom options
java -jar target/mqtt-java-1.0.0.jar subscriber \
  --broker localhost \
  --port 1883 \
  --topic mqtt-demo/all \
  --qos 1 \
  --encoding json
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
- `--encoding`: Encoding format - json, msgpack, cbor, or protobuf (default: json)

### Subscriber Options

- `--broker`: MQTT broker hostname (default: localhost)
- `--port`: MQTT broker port (default: 1883)
- `--topic`: MQTT topic (default: mqtt-demo/all)
- `--qos`: Quality of Service level - 0, 1, or 2 (default: 1)
- `--encoding`: Encoding format - json, msgpack, cbor, or protobuf (default: json)

## Message Format

The Java implementation uses the same sensor data format as other languages:

```java
public class SensorData {
    private double timestamp;
    private String sensorId;
    private double temperature;
    private double humidity;
    private double pressure;
    
    // Optional fields for larger payloads
    private Location location;
    private String status;
    private Double batteryLevel;
    private Integer signalStrength;
    private List<Double> sensorReadings;
    private Metadata metadata;
    private String additionalData;
}
```

## Performance

The Java implementation provides:

- **High performance**: JVM with JIT compilation for optimal speed
- **Memory efficiency**: Automatic garbage collection with optimized memory management
- **Fast serialization**: High-performance JSON/MessagePack/CBOR encoding/decoding
- **Enterprise features**: Built-in support for enterprise applications

## Benchmarking

Use the benchmark harness to test performance:

```bash
# From the project root
python3 benchmarks/benchmark.py --languages java --encodings json msgpack cbor protobuf
```

## Error Handling

The implementation includes comprehensive error handling for:

- Network connectivity issues
- Serialization/deserialization errors
- MQTT protocol errors
- Invalid command-line arguments
- Exception handling with try-catch blocks

## Memory Management

The implementation uses Java's automatic memory management:

- Garbage collection for automatic cleanup
- Efficient memory allocation
- No manual memory management required
- Optimized for performance

## Thread Safety

The MQTT client library handles thread safety internally:

- Callback functions are called from the MQTT client thread
- Shared data access is protected
- Async operations are properly synchronized
- Thread-safe collections and operations

## Java Features

The implementation uses modern Java features:

- Java 11+ with modern language features
- Jackson annotations for JSON serialization
- Maven for dependency management
- Picocli for command-line parsing
- Exception handling with try-catch
- Generic collections and type safety

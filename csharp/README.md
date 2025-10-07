# C# MQTT Implementation

C# implementation of MQTT publisher and subscriber with JSON encoding support.

## Features

- **Publisher**: Publishes sensor data with configurable payload sizes and QoS levels
- **Subscriber**: Subscribes to MQTT topics and decodes messages
- **Encoding**: JSON support (MessagePack support can be added)
- **Payload Sizes**: Small (~128B), Medium (~2KB), Large (~64KB)
- **QoS Levels**: 0, 1, and 2
- **Timing**: Publish and receive latency measurements

## Dependencies

- `MQTTnet` - High-performance MQTT client library for .NET
- `System.Text.Json` - High-performance JSON serialization
- `CommandLineParser` - Command-line argument parsing

## Installation

```bash
# Install .NET 6.0 SDK
# https://dotnet.microsoft.com/download/dotnet/6.0

# Restore dependencies
dotnet restore
```

## Building

```bash
# Build the project
dotnet build

# Build in release mode
dotnet build --configuration Release
```

## Usage

### Publisher

```bash
# Basic usage
dotnet run --project MQTTComparison.csproj -- --publisher

# With custom options
dotnet run --project MQTTComparison.csproj -- --publisher \
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
dotnet run --project MQTTComparison.csproj -- --subscriber

# With custom options
dotnet run --project MQTTComparison.csproj -- --subscriber \
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

The C# implementation uses the same sensor data format as other languages:

```csharp
public class SensorData
{
    public double Timestamp { get; set; }
    public string SensorId { get; set; } = string.Empty;
    public double Temperature { get; set; }
    public double Humidity { get; set; }
    public double Pressure { get; set; }
    
    // Optional fields for larger payloads
    public Location? Location { get; set; }
    public string? Status { get; set; }
    public double? BatteryLevel { get; set; }
    public int? SignalStrength { get; set; }
    public List<double>? SensorReadings { get; set; }
    public Metadata? Metadata { get; set; }
    public string? AdditionalData { get; set; }
}
```

## Performance

The C# implementation provides:

- **High performance**: .NET 6.0 with JIT compilation for optimal speed
- **Memory efficiency**: Automatic garbage collection with optimized memory management
- **Fast serialization**: High-performance JSON encoding/decoding with System.Text.Json
- **Async support**: Built-in async/await support for non-blocking operations

## Benchmarking

Use the benchmark harness to test performance:

```bash
# From the project root
python3 benchmarks/benchmark.py --languages csharp --encodings json
```

## Error Handling

The implementation includes comprehensive error handling for:

- Network connectivity issues
- JSON parsing errors
- MQTT protocol errors
- Invalid command-line arguments
- Exception handling with try-catch blocks

## Memory Management

The implementation uses .NET's automatic memory management:

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

## C# Features

The implementation uses modern C# features:

- C# 10.0 with nullable reference types
- Async/await for asynchronous operations
- LINQ for data manipulation
- Properties and auto-implemented properties
- Exception handling
- Generic collections
- JSON serialization with System.Text.Json
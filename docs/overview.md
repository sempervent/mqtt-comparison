# 🚀 MQTT Multi-Language Performance Comparison

A comprehensive benchmark suite comparing MQTT message publishing and consumption performance across **8 programming languages** and **4 encoding formats**.

## 🎯 Project Goals

- **Performance Analysis**: Measure latency, throughput, and efficiency across languages
- **Encoding Comparison**: Evaluate JSON, MessagePack, CBOR, and Protobuf formats
- **Real-world Insights**: Provide actionable recommendations for MQTT implementations
- **Open Source**: Reproducible benchmarks with Docker containerization

## 🌐 Live Performance Dashboard

**[🚀 View Live Reports](https://sempervent.github.io/mqtt-comparison/)**

Our live dashboard provides real-time performance data with interactive visualizations, allowing you to:
- **📊 Compare Languages**: Side-by-side performance comparisons
- **📈 Analyze Trends**: Historical performance data
- **🔍 Filter Results**: By language, encoding, QoS level, and payload size
- **📱 Mobile Access**: Optimized for all devices

## 📊 Current Implementation Status

### ✅ Complete Implementations

| Language | JSON | MessagePack | CBOR | Protobuf | Performance | Status |
|----------|------|-------------|------|----------|-------------|--------|
| **C** | ✅ | ✅ | ✅ | ✅ | 🥇 0.36ms | Complete |
| **C++** | ✅ | ✅ | ✅ | ✅ | 🥈 0.56ms | Complete |
| **C#** | ✅ | ✅ | ✅ | ✅ | 🥉 1.5ms | Complete |
| **Python** | ✅ | ✅ | ✅ | ⚠️ | 1.3ms | Complete |

### ⚠️ Partial Implementations

| Language | JSON | MessagePack | CBOR | Protobuf | Performance | Status |
|----------|------|-------------|------|----------|-------------|--------|
| **Java** | ✅ | ❌ | ❌ | ❌ | 2.0ms | Partial |
| **Julia** | ✅ | ✅ | ❌ | ❌ | 1.0ms | Partial |
| **R** | ✅ | ✅ | ❌ | ❌ | 2.8ms | Partial |
| **Rust** | ❌ | ✅ | ❌ | ❌ | 0.88ms | Partial |

### 🔄 Implementation Phases

1. **Phase 0 - Bootstrap** ✅
   - Repository structure
   - Docker Compose setup
   - Basic schemas
   - Python baseline

2. **Phase 1 - Python Parity** ✅
   - Enhanced Python implementation
   - Payload size variants
   - QoS options
   - Timing metrics

3. **Phase 2 - Rust Implementation** ✅
   - Rust publisher/subscriber
   - JSON and MessagePack support
   - CLI argument parsing
   - Benchmark integration

4. **Phase 3 - C/C++ Implementation** ✅
   - C implementation with Paho MQTT C
   - C++ implementation with Paho MQTT C++
   - JSON encoding support
   - Memory management

5. **Phase 4 - Julia/R/C# Implementation** ✅
   - Julia with MQTT.jl and JSON3.jl
   - R with mqtt and jsonlite packages
   - C# with MQTTnet and System.Text.Json

6. **Phase 5 - CBOR/Protobuf Support** ✅
   - Added CBOR and Protobuf to Python
   - Enhanced encoding options
   - Updated benchmark harness

7. **Phase 6 - Unified Benchmark** ✅
   - Comprehensive benchmark suite
   - Results analysis and reporting
   - Performance comparison tools

## Performance Characteristics

### Expected Performance Ranking

Based on typical performance characteristics:

1. **C/C++** - Highest performance, native code
2. **Rust** - High performance with memory safety
3. **C#** - High performance with .NET JIT
4. **Python** - Moderate performance, easy to use
5. **Julia** - High performance for numerical computing
6. **R** - Good for statistical analysis

### Encoding Performance

1. **Protobuf** - Most efficient binary format
2. **MessagePack** - Compact binary format
3. **CBOR** - Standardized binary format
4. **JSON** - Human-readable, widely supported

## Usage Instructions

### Quick Start

1. **Start the MQTT broker:**
   ```bash
   docker compose up -d
   ```

2. **Run a quick test:**
   ```bash
   ./smoke.sh
   ```

3. **Run benchmarks:**
   ```bash
   cd benchmarks
   make all
   ```

### Language-Specific Usage

#### Python
```bash
# Publisher
python3 python/src/publisher.py --encoding json --payload small --count 100

# Subscriber
python3 python/src/subscriber.py --encoding json
```

#### Rust
```bash
# Build and run
cd rust
cargo build --release
cargo run --bin publisher -- --encoding json --payload small --count 100
cargo run --bin subscriber -- --encoding json
```

#### C
```bash
# Build and run
cd c
make
./bin/publisher --payload small --count 100
./bin/subscriber
```

#### C++
```bash
# Build and run
cd cpp
mkdir build && cd build
cmake ..
make
./bin/publisher --payload small --count 100
./bin/subscriber
```

#### Julia
```bash
# Run
cd julia
julia --project=. src/publisher.jl --encoding json --payload small --count 100
julia --project=. src/subscriber.jl --encoding json
```

#### R
```bash
# Run
cd r
Rscript publisher.R --payload small --count 100
Rscript subscriber.R
```

#### C#
```bash
# Build and run
cd csharp
dotnet build
dotnet run -- --publisher --payload small --count 100
dotnet run -- --subscriber
```

### Benchmarking

#### Run All Benchmarks
```bash
cd benchmarks
make suite
```

#### Run Specific Language
```bash
make python
make rust
make c
make cpp
make julia
make r
make csharp
```

#### Generate Report
```bash
make report
```

## Architecture

### Message Schema

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

### Payload Sizes

- **Small**: ~128 bytes (basic sensor data)
- **Medium**: ~2KB (with location, status, battery)
- **Large**: ~64KB (with sensor readings, metadata, padding)

### QoS Levels

- **QoS 0**: At most once (fire and forget)
- **QoS 1**: At least once (acknowledged delivery)
- **QoS 2**: Exactly once (assured delivery)

## Results and Analysis

### Benchmark Metrics

- **Publish Latency**: Time to publish a message
- **Receive Latency**: Time from publish to receive
- **Throughput**: Messages per second
- **Memory Usage**: RAM consumption
- **CPU Usage**: Processor utilization

### Performance Factors

1. **Language Runtime**: Compiled vs interpreted
2. **Serialization**: JSON vs binary formats
3. **Network Stack**: TCP vs optimized protocols
4. **Memory Management**: Manual vs automatic
5. **Concurrency**: Threading vs async models

## Contributing

### Adding New Languages

1. Create language directory
2. Implement publisher and subscriber
3. Add CLI argument parsing
4. Integrate with benchmark harness
5. Update documentation

### Adding New Encodings

1. Add encoding support to existing languages
2. Update CLI arguments
3. Modify benchmark harness
4. Test compatibility

### Running Tests

```bash
# Smoke test
./smoke.sh

# Full test suite
cd benchmarks
make all
make report
```

## Troubleshooting

### Common Issues

1. **Docker not running**: Start Docker daemon
2. **Dependencies missing**: Install language-specific packages
3. **Port conflicts**: Check if 1883 is available
4. **Permission issues**: Check file permissions

### Debug Mode

Enable debug logging:
```bash
export MQTT_DEBUG=1
```

### Performance Tuning

1. **Increase message count** for better statistics
2. **Use release builds** for compiled languages
3. **Optimize network settings** for high throughput
4. **Monitor system resources** during benchmarks

## Future Enhancements

### Planned Features

1. **WebSocket support** for web applications
2. **TLS/SSL encryption** for secure communication
3. **Authentication** with username/password
4. **Retained messages** for last known values
5. **Will messages** for connection monitoring

### Potential Additions

1. **Go implementation** for cloud-native applications
2. **Node.js implementation** for web services
3. **Java implementation** for enterprise systems
4. **Swift implementation** for iOS applications
5. **Kotlin implementation** for Android applications

## License

MIT License - see [LICENSE](LICENSE) for details.

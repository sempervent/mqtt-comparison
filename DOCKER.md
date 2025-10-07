# Docker Setup for MQTT Comparison

This document describes how to use Docker and Docker Compose to run the MQTT comparison project in containerized environments.

## Quick Start

### Prerequisites

- Docker 20.10+
- Docker Compose 2.0+

### Basic Usage

```bash
# Start MQTT broker only
docker compose --profile broker up -d

# Test a specific language
./scripts/run-language-test.sh python json small 1 10

# Run all tests
./scripts/run-all-tests.sh

# Run comprehensive benchmark
./scripts/run-benchmark.sh
```

## Docker Compose Profiles

The project uses Docker Compose profiles to organize different services:

### Available Profiles

- `broker` - MQTT broker only
- `python` - Python implementation
- `java` - Java implementation  
- `rust` - Rust implementation
- `c` - C implementation
- `cpp` - C++ implementation
- `julia` - Julia implementation
- `r` - R implementation
- `csharp` - C# implementation
- `benchmark` - Benchmark harness
- `all` - All services
- `test` - Test environment

### Profile Usage

```bash
# Start specific language
docker compose --profile python up -d

# Start multiple languages
docker compose --profile python --profile java up -d

# Start everything
docker compose --profile all up -d
```

## Language-Specific Testing

### Python

```bash
# Test Python with JSON
./scripts/run-language-test.sh python json small 1 10

# Test Python with MessagePack
./scripts/run-language-test.sh python msgpack medium 2 50
```

### Java

```bash
# Test Java with JSON
./scripts/run-language-test.sh java json small 1 10

# Test Java with CBOR
./scripts/run-language-test.sh java cbor large 2 100
```

### Rust

```bash
# Test Rust with JSON
./scripts/run-language-test.sh rust json small 1 10

# Test Rust with MessagePack
./scripts/run-language-test.sh rust msgpack medium 1 50
```

### C/C++

```bash
# Test C implementation
./scripts/run-language-test.sh c json small 1 10

# Test C++ implementation
./scripts/run-language-test.sh cpp json small 1 10
```

### Julia/R/C#

```bash
# Test Julia
./scripts/run-language-test.sh julia json small 1 10

# Test R
./scripts/run-language-test.sh r json small 1 10

# Test C#
./scripts/run-language-test.sh csharp json small 1 10
```

## Benchmarking

### Quick Benchmark

```bash
# Run benchmark for specific language
docker compose run --rm benchmark \
  --languages python \
  --encodings json msgpack \
  --payloads small medium \
  --qos 1 2 \
  --count 100
```

### Comprehensive Benchmark

```bash
# Run full benchmark suite
./scripts/run-benchmark.sh
```

### Custom Benchmark

```bash
# Custom benchmark configuration
docker compose run --rm benchmark \
  --languages python java rust \
  --encodings json msgpack cbor \
  --payloads small large \
  --qos 0 1 2 \
  --count 1000
```

## Docker Images

### Language Images

Each language has its own Docker image:

- `mqtt-comparison-python-publisher` - Python publisher
- `mqtt-comparison-python-subscriber` - Python subscriber
- `mqtt-comparison-java-publisher` - Java publisher
- `mqtt-comparison-java-subscriber` - Java subscriber
- `mqtt-comparison-rust-publisher` - Rust publisher
- `mqtt-comparison-rust-subscriber` - Rust subscriber
- `mqtt-comparison-c-publisher` - C publisher
- `mqtt-comparison-c-subscriber` - C subscriber
- `mqtt-comparison-cpp-publisher` - C++ publisher
- `mqtt-comparison-cpp-subscriber` - C++ subscriber
- `mqtt-comparison-julia-publisher` - Julia publisher
- `mqtt-comparison-julia-subscriber` - Julia subscriber
- `mqtt-comparison-r-publisher` - R publisher
- `mqtt-comparison-r-subscriber` - R subscriber
- `mqtt-comparison-csharp-publisher` - C# publisher
- `mqtt-comparison-csharp-subscriber` - C# subscriber

### Benchmark Image

- `mqtt-comparison-benchmark` - Benchmark harness

## Environment Variables

### MQTT Configuration

- `MQTT_BROKER` - MQTT broker hostname (default: mosquitto)
- `MQTT_PORT` - MQTT broker port (default: 1883)

### Test Configuration

- `ENCODING` - Message encoding (json, msgpack, cbor, protobuf)
- `PAYLOAD_SIZE` - Payload size (small, medium, large)
- `QOS` - Quality of Service (0, 1, 2)
- `MESSAGE_COUNT` - Number of messages to send

## Volumes

### Results Volume

The benchmark results are stored in the `./results` directory, which is mounted as a volume in the benchmark container.

### Broker Data

- `mosquitto_data` - MQTT broker data persistence
- `mosquitto_logs` - MQTT broker logs

## Networking

All containers are connected to the default Docker network, allowing them to communicate using service names:

- `mosquitto` - MQTT broker
- `python-publisher` - Python publisher
- `python-subscriber` - Python subscriber
- etc.

## Troubleshooting

### Common Issues

1. **Broker not ready**: Wait a few seconds after starting the broker
2. **Connection refused**: Check if the broker is running
3. **Build failures**: Ensure all dependencies are available

### Debug Commands

```bash
# Check running containers
docker compose ps

# View logs
docker compose logs mosquitto
docker compose logs python-publisher

# Enter container
docker compose exec python-publisher bash

# Clean up
docker compose down --volumes --remove-orphans
```

### Performance Tuning

For better performance:

1. **Increase memory limits**:
   ```yaml
   deploy:
     resources:
       limits:
         memory: 2G
   ```

2. **Use host networking**:
   ```yaml
   network_mode: host
   ```

3. **Optimize Docker settings**:
   - Increase Docker memory limit
   - Use SSD storage
   - Enable Docker BuildKit

## Development

### Building Images

```bash
# Build all images
docker compose build

# Build specific language
docker compose build python-publisher

# Build with no cache
docker compose build --no-cache
```

### Testing Changes

```bash
# Test after changes
./scripts/run-language-test.sh python json small 1 5

# Rebuild and test
docker compose build python-publisher
./scripts/run-language-test.sh python json small 1 5
```

## Production Deployment

### Scaling

```bash
# Scale subscribers
docker compose up -d --scale python-subscriber=3

# Scale publishers
docker compose up -d --scale python-publisher=2
```

### Monitoring

```bash
# Monitor resource usage
docker stats

# Monitor logs
docker compose logs -f
```

## Security

### Network Security

- All containers run in isolated networks
- No external ports exposed except MQTT broker
- Internal communication only

### Data Security

- No sensitive data in containers
- Results stored in local volumes
- Clean shutdown removes temporary data

## Best Practices

1. **Use profiles** to organize services
2. **Clean up** after testing
3. **Monitor resources** during benchmarks
4. **Use volumes** for persistent data
5. **Test incrementally** before full benchmarks

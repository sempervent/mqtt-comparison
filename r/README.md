# R MQTT Implementation

R implementation of MQTT publisher and subscriber with JSON encoding support.

## Features

- **Publisher**: Publishes sensor data with configurable payload sizes and QoS levels
- **Subscriber**: Subscribes to MQTT topics and decodes messages
- **Encoding**: JSON support (MessagePack support can be added)
- **Payload Sizes**: Small (~128B), Medium (~2KB), Large (~64KB)
- **QoS Levels**: 0, 1, and 2
- **Timing**: Publish and receive latency measurements

## Dependencies

- `jsonlite` - JSON parsing and generation
- `mqtt` - MQTT client library for R

## Installation

```bash
# Install R dependencies
Rscript -e "install.packages(c('jsonlite', 'mqtt'), repos='https://cran.r-project.org')"
```

## Usage

### Publisher

```bash
# Basic usage
Rscript publisher.R

# With custom options
Rscript publisher.R \
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
Rscript subscriber.R

# With custom options
Rscript subscriber.R \
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

The R implementation uses the same sensor data format as other languages:

```r
# Base sensor data structure
data <- list(
  timestamp = as.numeric(Sys.time()),
  sensor_id = "sensor_001",
  temperature = 23.5,
  humidity = 45.2,
  pressure = 1013.25
)

# Optional fields for larger payloads
if (payload_size == "medium") {
  data$location <- list(lat = 40.7128, lon = -74.0060, altitude = 10.5)
  data$status <- "active"
  data$battery_level <- 85.5
  data$signal_strength <- -65
  data$additional_data <- paste(rep("x", 1500), collapse = "")
}
```

## Performance

The R implementation provides:

- **Statistical computing**: R's strength in data analysis and statistics
- **Memory efficiency**: Efficient memory management with garbage collection
- **Fast serialization**: High-performance JSON encoding/decoding with jsonlite
- **Data analysis**: Built-in support for statistical operations

## Benchmarking

Use the benchmark harness to test performance:

```bash
# From the project root
python3 benchmarks/benchmark.py --languages r --encodings json
```

## Error Handling

The implementation includes comprehensive error handling for:

- Network connectivity issues
- JSON parsing errors
- MQTT protocol errors
- Invalid command-line arguments
- Interrupt handling (Ctrl+C)

## Memory Management

The implementation uses R's automatic memory management:

- Garbage collection for automatic cleanup
- Efficient memory allocation
- No manual memory management required
- Optimized for statistical computing

## Thread Safety

The MQTT client library handles thread safety internally:

- Callback functions are called from the MQTT client thread
- Shared data access is protected
- Signal handlers are thread-safe
- Async operations are properly synchronized

## R Features

The implementation uses modern R features:

- Functional programming with closures
- List data structures for flexible data representation
- JSON serialization with jsonlite
- Command-line argument parsing
- Error handling with tryCatch
- Signal handling with on.exit
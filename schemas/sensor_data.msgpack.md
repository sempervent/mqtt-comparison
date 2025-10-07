# MessagePack Schema for Sensor Data

This document describes the MessagePack encoding format for sensor data messages used in the MQTT comparison project.

## MessagePack Format

MessagePack is a binary serialization format that is more compact than JSON while maintaining similar data structures.

## Sensor Data Structure

The sensor data is encoded as a MessagePack map with the following fields:

### Required Fields

- `timestamp` (float64): Unix timestamp in seconds
- `sensor_id` (string): Unique sensor identifier
- `temperature` (float64): Temperature in Celsius
- `humidity` (float64): Humidity percentage
- `pressure` (float64): Atmospheric pressure in hPa

### Optional Fields (for larger payloads)

- `location` (map): Geographic location
  - `lat` (float64): Latitude
  - `lon` (float64): Longitude
  - `altitude` (float64): Altitude in meters
- `status` (string): Sensor status (e.g., "active", "inactive")
- `battery_level` (float64): Battery level percentage
- `signal_strength` (int32): Signal strength in dBm
- `sensor_readings` (array): Array of sensor readings (float64)
- `metadata` (map): Sensor metadata
  - `firmware_version` (string): Firmware version
  - `hardware_id` (string): Hardware identifier
  - `calibration_date` (string): Calibration date
  - `last_maintenance` (string): Last maintenance date
- `additional_data` (string): Additional padding data

## Payload Sizes

### Small Payload (~128 bytes)
Contains only the required fields:
```msgpack
{
  "timestamp": 1234567890.123,
  "sensor_id": "sensor_001",
  "temperature": 23.5,
  "humidity": 45.2,
  "pressure": 1013.25
}
```

### Medium Payload (~2KB)
Includes location, status, and battery information:
```msgpack
{
  "timestamp": 1234567890.123,
  "sensor_id": "sensor_001",
  "temperature": 23.5,
  "humidity": 45.2,
  "pressure": 1013.25,
  "location": {
    "lat": 40.7128,
    "lon": -74.0060,
    "altitude": 10.5
  },
  "status": "active",
  "battery_level": 85.5,
  "signal_strength": -65,
  "additional_data": "x" * 1500
}
```

### Large Payload (~64KB)
Includes all fields with extensive sensor readings and metadata:
```msgpack
{
  "timestamp": 1234567890.123,
  "sensor_id": "sensor_001",
  "temperature": 23.5,
  "humidity": 45.2,
  "pressure": 1013.25,
  "location": {
    "lat": 40.7128,
    "lon": -74.0060,
    "altitude": 10.5
  },
  "status": "active",
  "battery_level": 85.5,
  "signal_strength": -65,
  "sensor_readings": [/* 100 float64 values */],
  "metadata": {
    "firmware_version": "1.2.3",
    "hardware_id": "HW-001",
    "calibration_date": "2024-01-01",
    "last_maintenance": "2024-06-01"
  },
  "additional_data": "x" * 60000
}
```

## Implementation Notes

### Python
```python
import msgpack

# Encode
data = {"timestamp": 1234567890.123, "sensor_id": "sensor_001", ...}
packed = msgpack.packb(data)

# Decode
unpacked = msgpack.unpackb(packed, raw=False)
```

### Rust
```rust
use rmp_serde;

// Encode
let data = SensorData { timestamp: 1234567890.123, sensor_id: "sensor_001".to_string(), ... };
let packed = rmp_serde::to_vec(&data).unwrap();

// Decode
let unpacked: SensorData = rmp_serde::from_slice(&packed).unwrap();
```

## Performance Characteristics

MessagePack typically provides:
- **Size**: 20-30% smaller than JSON
- **Speed**: 2-3x faster encoding/decoding than JSON
- **Compatibility**: Good cross-language support
- **Schema**: Self-describing like JSON

## Advantages

1. **Compact**: Binary format is more space-efficient than JSON
2. **Fast**: Optimized for speed in encoding/decoding
3. **Cross-platform**: Works across different programming languages
4. **Schema-less**: No need for separate schema files
5. **Type-safe**: Preserves data types better than JSON

## Disadvantages

1. **Human-readable**: Not easily readable by humans
2. **Debugging**: Harder to debug than text formats
3. **Schema evolution**: Less flexible than Protocol Buffers
4. **Validation**: No built-in schema validation

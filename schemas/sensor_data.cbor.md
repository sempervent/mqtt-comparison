# CBOR Schema for Sensor Data

This document describes the CBOR (Concise Binary Object Representation) encoding format for sensor data messages used in the MQTT comparison project.

## CBOR Format

CBOR is a binary data format designed for small code size, small message size, and extensibility. It is standardized in RFC 7049 and RFC 8949.

## Sensor Data Structure

The sensor data is encoded as a CBOR map with the following fields:

### Required Fields

- `timestamp` (float64): Unix timestamp in seconds
- `sensor_id` (text string): Unique sensor identifier
- `temperature` (float64): Temperature in Celsius
- `humidity` (float64): Humidity percentage
- `pressure` (float64): Atmospheric pressure in hPa

### Optional Fields (for larger payloads)

- `location` (map): Geographic location
  - `lat` (float64): Latitude
  - `lon` (float64): Longitude
  - `altitude` (float64): Altitude in meters
- `status` (text string): Sensor status (e.g., "active", "inactive")
- `battery_level` (float64): Battery level percentage
- `signal_strength` (int32): Signal strength in dBm
- `sensor_readings` (array): Array of sensor readings (float64)
- `metadata` (map): Sensor metadata
  - `firmware_version` (text string): Firmware version
  - `hardware_id` (text string): Hardware identifier
  - `calibration_date` (text string): Calibration date
  - `last_maintenance` (text string): Last maintenance date
- `additional_data` (text string): Additional padding data

## Payload Sizes

### Small Payload (~128 bytes)
Contains only the required fields:
```cbor
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
```cbor
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
```cbor
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
import cbor2

# Encode
data = {"timestamp": 1234567890.123, "sensor_id": "sensor_001", ...}
packed = cbor2.dumps(data)

# Decode
unpacked = cbor2.loads(packed)
```

### Rust
```rust
use serde_cbor;

// Encode
let data = SensorData { timestamp: 1234567890.123, sensor_id: "sensor_001".to_string(), ... };
let packed = serde_cbor::to_vec(&data).unwrap();

// Decode
let unpacked: SensorData = serde_cbor::from_slice(&packed).unwrap();
```

## Performance Characteristics

CBOR typically provides:
- **Size**: 15-25% smaller than JSON
- **Speed**: 1.5-2x faster encoding/decoding than JSON
- **Compatibility**: Excellent cross-language support
- **Schema**: Self-describing like JSON
- **Standards**: IETF standard (RFC 8949)

## Advantages

1. **Standardized**: IETF standard with excellent documentation
2. **Compact**: Binary format is more space-efficient than JSON
3. **Fast**: Optimized for speed in encoding/decoding
4. **Cross-platform**: Works across different programming languages
5. **Schema-less**: No need for separate schema files
6. **Type-safe**: Preserves data types better than JSON
7. **Extensible**: Supports custom data types and tags

## Disadvantages

1. **Human-readable**: Not easily readable by humans
2. **Debugging**: Harder to debug than text formats
3. **Schema evolution**: Less flexible than Protocol Buffers
4. **Validation**: No built-in schema validation
5. **Learning curve**: Less familiar than JSON

## Comparison with Other Formats

| Format | Size | Speed | Human-readable | Standards | Schema |
|--------|------|-------|----------------|-----------|--------|
| JSON   | 100% | 1x    | Yes            | RFC 7159  | No     |
| MessagePack | 70-80% | 2-3x | No | Community | No |
| CBOR   | 75-85% | 1.5-2x | No | RFC 8949 | No |
| Protobuf | 60-70% | 3-4x | No | Google | Yes |

## Use Cases

CBOR is particularly well-suited for:
- IoT applications with constrained resources
- Web APIs requiring efficient serialization
- Inter-service communication
- Data storage and archival
- Real-time data streaming

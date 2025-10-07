# Quick Reference

## Common Commands

### Start/Stop Broker

```bash
# Start
docker compose up -d

# Stop
docker compose down

# Restart
docker compose restart mosquitto

# View logs
docker compose logs -f mosquitto
```

### Python Publisher/Subscriber

```bash
# Publisher (JSON)
python3 python/src/publisher.py --encoding json --count 10

# Publisher (MessagePack)
python3 python/src/publisher.py --encoding msgpack --count 10

# Subscriber (JSON)
python3 python/src/subscriber.py --encoding json

# Subscriber (MessagePack)
python3 python/src/subscriber.py --encoding msgpack
```

### Run Benchmarks

```bash
# All Python encodings
python3 benchmarks/benchmark.py --languages python

# Specific encoding
python3 benchmarks/benchmark.py --languages python --encodings json

# Custom message count
python3 benchmarks/benchmark.py --count 1000
```

### Phase Management

```bash
# Create phase summary
./summarize_phase.py phase_name "Summary text"

# Run smoke test
./smoke.sh
```

### Using Make

```bash
make help          # Show all commands
make start         # Start broker
make stop          # Stop broker
make smoke         # Run smoke test
make install-python # Install Python deps
make benchmark     # Run benchmarks
make clean         # Clean generated files
```

## MQTT Topics

- `sensors/data` - Default sensor data topic
- `sensors/temp` - Temperature sensors
- `sensors/humidity` - Humidity sensors
- Custom topics supported via `--topic` flag

## Broker Configuration

- **Host**: localhost
- **Port**: 1883 (MQTT)
- **WebSocket Port**: 9001 (future)
- **Authentication**: None (allow_anonymous true)
- **Persistence**: Enabled

## File Locations

```
mqtt-comparison/
├── docker-compose.yml          # Broker config
├── mosquitto/config/           # Mosquitto settings
├── schemas/                    # Message schemas
├── python/src/                 # Python implementation
├── benchmarks/                 # Benchmark harness
├── docs/_summaries/            # Phase summaries
└── Makefile                    # Make commands
```

## Python API Quick Start

### Publisher

```python
from publisher import SensorDataPublisher

pub = SensorDataPublisher(broker="localhost", encoding="json")
pub.connect()

data = pub.create_sensor_data("sensor_001")
pub.publish("sensors/data", data)

pub.disconnect()
```

### Subscriber

```python
from subscriber import SensorDataSubscriber

sub = SensorDataSubscriber(broker="localhost", encoding="json")
sub.connect("sensors/data")
sub.loop()  # Blocks until interrupted
```

## Troubleshooting

### Broker won't start
```bash
docker compose down
docker compose up -d
docker compose logs mosquitto
```

### Connection refused
```bash
# Check broker is running
docker compose ps

# Check port is available
netstat -ln | grep 1883
```

### Python dependencies
```bash
cd python
pip install -r requirements.txt
```

### Clean slate
```bash
make clean
docker compose down -v
docker compose up -d
```

## Environment Variables

Set these for custom configurations:

```bash
export MQTT_BROKER=localhost
export MQTT_PORT=1883
export MQTT_TOPIC=sensors/data
```

## Performance Tips

1. Use MessagePack for better performance
2. Increase message count for accurate benchmarks
3. Run broker on different host for realistic tests
4. Monitor system resources during benchmarks
5. Use QoS 0 for maximum throughput

## Next Steps

1. Complete Rust implementation (Phase 3)
2. Add C/C++ implementations (Phase 4)
3. Implement remaining languages (Phase 5)
4. Add CBOR and Protobuf (Phase 6)
5. Enhance benchmarks (Phase 7)

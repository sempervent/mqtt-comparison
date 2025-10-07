# Python MQTT Implementation

Python implementation of MQTT pub/sub with multiple encoding support.

## Features

- JSON encoding
- MessagePack encoding
- Publisher and Subscriber implementations
- Command-line interface

## Setup

```bash
cd python
pip install -r requirements.txt
```

## Usage

### Publisher

```bash
# Publish with JSON encoding
python3 src/publisher.py --encoding json --count 5

# Publish with MessagePack encoding
python3 src/publisher.py --encoding msgpack --count 5

# Custom topic and sensor ID
python3 src/publisher.py --topic sensors/temp --sensor-id temp_001
```

### Subscriber

```bash
# Subscribe with JSON encoding
python3 src/subscriber.py --encoding json

# Subscribe with MessagePack encoding
python3 src/subscriber.py --encoding msgpack

# Custom topic
python3 src/subscriber.py --topic sensors/temp
```

## Testing

Make sure the MQTT broker is running:

```bash
cd ..
docker-compose up -d
```

Then run publisher and subscriber in separate terminals:

```bash
# Terminal 1
python3 src/subscriber.py

# Terminal 2
python3 src/publisher.py --count 10
```

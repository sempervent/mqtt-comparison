# Development Guide

## Getting Started

### Prerequisites

- Docker and Docker Compose
- Python 3.11+
- Git

### Initial Setup

1. Clone the repository:
```bash
git clone https://github.com/sempervent/mqtt-comparison.git
cd mqtt-comparison
```

2. Run smoke test:
```bash
./smoke.sh
```

3. Start MQTT broker:
```bash
docker-compose up -d
```

4. Verify broker is running:
```bash
docker-compose ps
docker-compose logs mosquitto
```

## Phase Development

The project follows a phased development approach. Each phase builds on the previous one.

### Phase 1: Bootstrap ✓

Core infrastructure setup:
- Docker Compose with Mosquitto
- Shared schemas
- Documentation structure
- CI/CD workflows
- Utility scripts

### Phase 2: Python

Python implementation with:
- JSON encoding
- MessagePack encoding
- Publisher/Subscriber
- Basic benchmarks

### Phase 3: Rust

Rust implementation with:
- JSON encoding
- MessagePack encoding
- Async runtime
- Performance optimizations

### Phase 4: C/C++

C and C++ implementations with:
- JSON encoding
- MessagePack encoding
- Manual memory management
- Performance focus

### Phase 5: Julia/R/C#

Implementations for:
- Julia (scientific computing)
- R (statistics)
- C# (.NET ecosystem)

### Phase 6: CBOR/Protobuf

Add additional encodings:
- CBOR support across all languages
- Protocol Buffers with schema
- Comparison with existing encodings

### Phase 7: Unified Benchmarks

Complete benchmark suite:
- Cross-language testing
- Performance analysis
- Documentation
- Final report

## Adding a New Language

1. Create language directory:
```bash
mkdir -p <language>/src
```

2. Create README.md with:
   - Features
   - Dependencies
   - Usage examples
   - Build instructions

3. Implement publisher:
   - Connect to MQTT broker
   - Encode messages
   - Publish to topic

4. Implement subscriber:
   - Connect to MQTT broker
   - Subscribe to topic
   - Decode messages

5. Add to benchmark harness:
```python
def run_<language>_benchmark(self, encoding, message_count):
    # Implementation
    pass
```

6. Test implementation:
```bash
# Terminal 1: Start subscriber
cd <language>
./subscriber --encoding json

# Terminal 2: Run publisher
./publisher --encoding json --count 10
```

7. Update documentation:
   - Add to main README
   - Document in ARCHITECTURE.md
   - Create phase summary

## Testing Guidelines

### Local Testing

Before committing:

1. Run smoke test:
```bash
./smoke.sh
```

2. Test MQTT broker:
```bash
docker-compose up -d
docker-compose logs
```

3. Test implementation:
```bash
# Run publisher/subscriber tests
cd python
python3 src/publisher.py --count 5
```

4. Run benchmarks:
```bash
cd benchmarks
python3 benchmark.py --count 100
```

### CI Testing

GitHub Actions will automatically:
- Run smoke tests
- Start MQTT broker
- Test implementations
- Verify structure

### Manual Testing

For thorough testing:

1. Start broker:
```bash
docker-compose up -d
```

2. Start subscriber:
```bash
cd python
python3 src/subscriber.py --encoding json &
SUB_PID=$!
```

3. Run publisher:
```bash
python3 src/publisher.py --encoding json --count 10
```

4. Verify output and cleanup:
```bash
kill $SUB_PID
```

## Code Style

### Python
- Follow PEP 8
- Use type hints
- Document with docstrings
- Use `black` for formatting

### Rust
- Follow Rust style guide
- Use `rustfmt`
- Document with `///` comments
- Run `clippy` for linting

### C/C++
- Follow project conventions
- Use consistent indentation
- Document functions
- Use `clang-format`

## Git Workflow

1. Create feature branch:
```bash
git checkout -b feature/add-rust-implementation
```

2. Make changes and test

3. Commit with descriptive message:
```bash
git commit -m "Add Rust pub/sub with JSON encoding"
```

4. Push and create PR:
```bash
git push origin feature/add-rust-implementation
```

## Phase Completion

After completing a phase:

1. Test all functionality
2. Update documentation
3. Create phase summary:
```bash
./summarize_phase.py <phase_name> "Phase completion summary"
```

4. Commit and push:
```bash
git add .
git commit -m "Complete Phase X: <phase_name>"
git push
```

## Troubleshooting

### MQTT Broker Won't Start

```bash
# Check Docker status
docker info

# Check logs
docker-compose logs mosquitto

# Restart broker
docker-compose restart mosquitto
```

### Connection Refused

```bash
# Verify broker is running
docker-compose ps

# Check port is available
netstat -ln | grep 1883

# Test connection
telnet localhost 1883
```

### Dependencies Issues

```bash
# Python
cd python
pip install -r requirements.txt

# Rust
cd rust
cargo build

# Clean and rebuild
cargo clean
cargo build
```

## Resources

- [MQTT Protocol](https://mqtt.org/)
- [Mosquitto Documentation](https://mosquitto.org/documentation/)
- [Paho MQTT Clients](https://www.eclipse.org/paho/)
- [MessagePack](https://msgpack.org/)
- [Protocol Buffers](https://protobuf.dev/)

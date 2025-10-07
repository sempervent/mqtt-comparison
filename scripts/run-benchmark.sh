#!/bin/bash
# Run comprehensive benchmark suite

echo "Running comprehensive MQTT benchmark suite..."

# Start broker
echo "Starting MQTT broker..."
docker compose --profile broker up -d

# Wait for broker to be ready
echo "Waiting for broker to be ready..."
sleep 5

# Run benchmark harness
echo "Running benchmark harness..."
docker compose run --rm benchmark \
  --languages python java rust c cpp julia r csharp \
  --encodings json msgpack cbor protobuf \
  --payloads small medium large \
  --qos 0 1 2 \
  --count 100

# Cleanup
echo "Cleaning up..."
docker compose --profile broker down

echo "Benchmark completed! Results saved to ./results/"

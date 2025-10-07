#!/bin/bash
# Run all language tests

echo "Running all MQTT language tests..."

# Start broker
echo "Starting MQTT broker..."
docker compose --profile broker up -d

# Wait for broker to be ready
echo "Waiting for broker to be ready..."
sleep 5

# Test each language
LANGUAGES=("python" "java" "rust" "c" "cpp" "julia" "r" "csharp")

for lang in "${LANGUAGES[@]}"; do
    echo "Testing $lang..."
    
    # Start subscriber
    docker compose --profile $lang up -d ${lang}-subscriber
    
    # Wait for subscriber to connect
    sleep 3
    
    # Run publisher
    docker compose run --rm ${lang}-publisher \
      --broker mosquitto \
      --port 1883 \
      --encoding json \
      --payload small \
      --qos 1 \
      --count 5
    
    # Cleanup language services
    docker compose --profile $lang down
    
    echo "$lang test completed!"
done

# Cleanup
echo "Cleaning up..."
docker compose --profile broker down

echo "All tests completed!"

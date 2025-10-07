#!/bin/bash
# Run tests for a specific language

LANGUAGE=${1:-python}
ENCODING=${2:-json}
PAYLOAD=${3:-small}
QOS=${4:-1}
COUNT=${5:-10}

echo "Running $LANGUAGE tests with $ENCODING encoding, $PAYLOAD payload, QoS $QOS, $COUNT messages..."

# Start broker
echo "Starting MQTT broker..."
docker compose --profile broker up -d

# Wait for broker to be ready
echo "Waiting for broker to be ready..."
sleep 5

# Start subscriber
echo "Starting $LANGUAGE subscriber..."
docker compose --profile $LANGUAGE up -d ${LANGUAGE}-subscriber

# Wait for subscriber to connect
sleep 3

# Run publisher
echo "Running $LANGUAGE publisher..."
docker compose run --rm ${LANGUAGE}-publisher \
  --broker mosquitto \
  --port 1883 \
  --encoding $ENCODING \
  --payload $PAYLOAD \
  --qos $QOS \
  --count $COUNT

# Cleanup
echo "Cleaning up..."
docker compose --profile $LANGUAGE down
docker compose --profile broker down

echo "Test completed!"

#!/bin/bash
# Start MQTT broker only

echo "Starting MQTT broker..."
docker compose --profile broker up -d

echo "MQTT broker is running at localhost:1883"
echo "To stop: docker compose down"

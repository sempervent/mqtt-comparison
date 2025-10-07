#!/bin/bash
# Test Docker setup

echo "Testing Docker setup for MQTT Comparison..."

# Check if Docker is running
if ! docker info > /dev/null 2>&1; then
    echo "❌ Docker is not running. Please start Docker Desktop."
    exit 1
fi

echo "✅ Docker is running"

# Test Docker Compose
if ! docker compose version > /dev/null 2>&1; then
    echo "❌ Docker Compose is not available"
    exit 1
fi

echo "✅ Docker Compose is available"

# Test broker startup
echo "Testing MQTT broker startup..."
if docker compose --profile broker up -d; then
    echo "✅ MQTT broker started successfully"
    
    # Wait for broker to be ready
    sleep 5
    
    # Test broker connectivity
    if docker compose exec mosquitto mosquitto_pub -h localhost -t test -m "hello" -p 1883; then
        echo "✅ MQTT broker is working"
    else
        echo "❌ MQTT broker connectivity test failed"
    fi
    
    # Cleanup
    docker compose --profile broker down
    echo "✅ Cleanup completed"
else
    echo "❌ Failed to start MQTT broker"
    exit 1
fi

echo "🎉 Docker setup test completed successfully!"

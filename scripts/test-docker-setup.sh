#!/bin/bash
# Test Docker setup for MQTT Comparison

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

# Test building a simple image
echo "Testing Python build..."
if docker compose build python-publisher; then
    echo "✅ Python build successful"
else
    echo "❌ Python build failed"
    exit 1
fi

# Test building Julia
echo "Testing Julia build..."
if docker compose build julia-publisher; then
    echo "✅ Julia build successful"
else
    echo "❌ Julia build failed"
    exit 1
fi

# Test broker startup
echo "Testing MQTT broker startup..."
if docker compose --profile broker up -d; then
    echo "✅ MQTT broker started successfully"
    
    # Wait for broker to be ready
    sleep 5
    
    # Cleanup
    docker compose --profile broker down
    echo "✅ Cleanup completed"
else
    echo "❌ Failed to start MQTT broker"
    exit 1
fi

echo "🎉 Docker setup test completed successfully!"
echo ""
echo "You can now run:"
echo "  ./scripts/run-language-test.sh python json small 1 10"
echo "  ./scripts/run-all-tests.sh"
echo "  ./scripts/run-benchmark.sh"

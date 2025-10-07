#!/bin/bash
# smoke.sh - Quick smoke test to verify basic setup

set -e

echo "=== MQTT Comparison Smoke Test ==="
echo ""

# Check for required files
echo "✓ Checking project structure..."
[ -f "docker-compose.yml" ] && echo "  - docker-compose.yml exists"
[ -d "schemas" ] && echo "  - schemas/ directory exists"
[ -d "docs/_summaries" ] && echo "  - docs/_summaries/ directory exists"
[ -f "summarize_phase.py" ] && echo "  - summarize_phase.py exists"

# Check Docker
echo ""
echo "✓ Checking Docker..."
if command -v docker &> /dev/null; then
    echo "  - Docker is installed"
    if docker info &> /dev/null; then
        echo "  - Docker daemon is running"
    else
        echo "  ⚠ Docker daemon is not running"
    fi
else
    echo "  ⚠ Docker is not installed"
fi

# Check Docker Compose
echo ""
echo "✓ Checking Docker Compose..."
if command -v docker compose &> /dev/null || docker compose version &> /dev/null 2>&1; then
    echo "  - Docker Compose is available"
else
    echo "  ⚠ Docker Compose is not available"
fi

# Check Python
echo ""
echo "✓ Checking Python..."
if command -v python3 &> /dev/null; then
    PYTHON_VERSION=$(python3 --version)
    echo "  - Python is installed: $PYTHON_VERSION"
else
    echo "  ⚠ Python 3 is not installed"
fi

echo ""
echo "=== Smoke Test Complete ==="
echo ""
echo "To start the MQTT broker:"
echo "  docker compose up -d"
echo ""
echo "To stop the MQTT broker:"
echo "  docker compose down"

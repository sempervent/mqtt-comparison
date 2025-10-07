.PHONY: help start stop restart logs clean test smoke install-python benchmark

help:
	@echo "MQTT Comparison - Make Commands"
	@echo ""
	@echo "Docker Commands:"
	@echo "  make start          - Start MQTT broker"
	@echo "  make stop           - Stop MQTT broker"
	@echo "  make restart        - Restart MQTT broker"
	@echo "  make logs           - View broker logs"
	@echo ""
	@echo "Development Commands:"
	@echo "  make test           - Run all tests"
	@echo "  make smoke          - Run smoke test"
	@echo "  make clean          - Clean generated files"
	@echo ""
	@echo "Python Commands:"
	@echo "  make install-python - Install Python dependencies"
	@echo "  make python-pub     - Run Python publisher (JSON)"
	@echo "  make python-sub     - Run Python subscriber (JSON)"
	@echo ""
	@echo "Benchmark Commands:"
	@echo "  make benchmark      - Run benchmarks"
	@echo ""

start:
	docker-compose up -d
	@echo "✓ MQTT broker started on localhost:1883"

stop:
	docker-compose down
	@echo "✓ MQTT broker stopped"

restart: stop start

logs:
	docker-compose logs -f mosquitto

clean:
	rm -rf mosquitto/data/*
	rm -rf mosquitto/log/*
	rm -f benchmark_results.json
	find . -type d -name __pycache__ -exec rm -rf {} + 2>/dev/null || true
	find . -type f -name "*.pyc" -delete 2>/dev/null || true
	@echo "✓ Cleaned generated files"

test: smoke
	@echo "✓ Tests passed"

smoke:
	./smoke.sh

install-python:
	cd python && pip install -r requirements.txt
	@echo "✓ Python dependencies installed"

python-pub:
	python3 python/src/publisher.py --encoding json --count 10

python-sub:
	python3 python/src/subscriber.py --encoding json

benchmark:
	python3 benchmarks/benchmark.py --languages python --encodings json msgpack --count 100

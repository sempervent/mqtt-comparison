# MQTT Comparison Benchmarks Makefile

.PHONY: help python rust c cpp julia r csharp java all clean

# Default target
help:
	@echo "MQTT Comparison Benchmarks"
	@echo ""
	@echo "Available targets:"
	@echo "  python    - Run Python benchmarks"
	@echo "  rust      - Run Rust benchmarks (not yet implemented)"
	@echo "  c         - Run C benchmarks (not yet implemented)"
	@echo "  cpp       - Run C++ benchmarks (not yet implemented)"
	@echo "  julia     - Run Julia benchmarks (not yet implemented)"
	@echo "  r         - Run R benchmarks (not yet implemented)"
	@echo "  csharp    - Run C# benchmarks (not yet implemented)"
	@echo "  java      - Run Java benchmarks (not yet implemented)"
	@echo "  all       - Run all available benchmarks"
	@echo "  clean     - Clean benchmark results"
	@echo ""
	@echo "Usage examples:"
	@echo "  make python                    # Run Python benchmarks"
	@echo "  make python ENCODINGS=json     # Run only JSON encoding"
	@echo "  make python PAYLOADS=small     # Run only small payloads"

# Variables
LANGUAGES ?= python
ENCODINGS ?= json msgpack
PAYLOADS ?= small medium large
QOS ?= 0 1 2
COUNT ?= 100
BROKER ?= localhost
PORT ?= 1883

# Python benchmarks
python:
	@echo "Running Python benchmarks..."
	python3 benchmark.py \
		--languages $(LANGUAGES) \
		--encodings $(ENCODINGS) \
		--payloads $(PAYLOADS) \
		--qos $(QOS) \
		--count $(COUNT) \
		--broker $(BROKER) \
		--port $(PORT) \
		--output results/python/benchmark_results.json

# Rust benchmarks
rust:
	@echo "Running Rust benchmarks..."
	python3 benchmark.py \
		--languages rust \
		--encodings $(ENCODINGS) \
		--payloads $(PAYLOADS) \
		--qos $(QOS) \
		--count $(COUNT) \
		--broker $(BROKER) \
		--port $(PORT) \
		--output results/rust/benchmark_results.json

# C benchmarks
c:
	@echo "Running C benchmarks..."
	python3 benchmark.py \
		--languages c \
		--encodings $(ENCODINGS) \
		--payloads $(PAYLOADS) \
		--qos $(QOS) \
		--count $(COUNT) \
		--broker $(BROKER) \
		--port $(PORT) \
		--output results/c/benchmark_results.json

# C++ benchmarks
cpp:
	@echo "Running C++ benchmarks..."
	python3 benchmark.py \
		--languages cpp \
		--encodings $(ENCODINGS) \
		--payloads $(PAYLOADS) \
		--qos $(QOS) \
		--count $(COUNT) \
		--broker $(BROKER) \
		--port $(PORT) \
		--output results/cpp/benchmark_results.json

# Julia benchmarks
julia:
	@echo "Running Julia benchmarks..."
	python3 benchmark.py \
		--languages julia \
		--encodings $(ENCODINGS) \
		--payloads $(PAYLOADS) \
		--qos $(QOS) \
		--count $(COUNT) \
		--broker $(BROKER) \
		--port $(PORT) \
		--output results/julia/benchmark_results.json

# R benchmarks
r:
	@echo "Running R benchmarks..."
	python3 benchmark.py \
		--languages r \
		--encodings $(ENCODINGS) \
		--payloads $(PAYLOADS) \
		--qos $(QOS) \
		--count $(COUNT) \
		--broker $(BROKER) \
		--port $(PORT) \
		--output results/r/benchmark_results.json

# C# benchmarks
csharp:
	@echo "Running C# benchmarks..."
	python3 benchmark.py \
		--languages csharp \
		--encodings $(ENCODINGS) \
		--payloads $(PAYLOADS) \
		--qos $(QOS) \
		--count $(COUNT) \
		--broker $(BROKER) \
		--port $(PORT) \
		--output results/csharp/benchmark_results.json

# Java benchmarks
java:
	@echo "Running Java benchmarks..."
	python3 benchmark.py \
		--languages java \
		--encodings $(ENCODINGS) \
		--payloads $(PAYLOADS) \
		--qos $(QOS) \
		--count $(COUNT) \
		--broker $(BROKER) \
		--port $(PORT) \
		--output results/java/benchmark_results.json

# Run all available benchmarks
all: python rust c cpp julia r csharp java
	@echo "All available benchmarks completed"

# Run complete benchmark suite
suite:
	@echo "Running complete benchmark suite..."
	python3 run_all.py

# Generate comprehensive report
report:
	@echo "Generating comprehensive report..."
	python3 analyze_results.py --output docs/benchmark_report.md

# Clean benchmark results
clean:
	@echo "Cleaning benchmark results..."
	rm -rf results/*.json
	rm -rf results/*/*.json
	@echo "Benchmark results cleaned"

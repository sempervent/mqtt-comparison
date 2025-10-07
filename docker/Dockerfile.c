# C MQTT Implementation Dockerfile
FROM gcc:12 as builder

WORKDIR /app

# Install dependencies
RUN apt-get update && apt-get install -y \
    libpaho-mqtt-dev \
    libjson-c-dev \
    libmsgpack-dev \
    libcbor-dev \
    protobuf-compiler \
    libprotobuf-dev \
    protobuf-c-compiler \
    libprotobuf-c-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy C source code
COPY c/src/ ./src/
COPY c/Makefile .
COPY schemas/ ./schemas/

# Generate protobuf files
RUN protoc --proto_path=./schemas --c_out=./src schemas/sensor_data.proto

# Build the project
RUN make -C .

# Runtime stage
FROM debian:bookworm-slim

WORKDIR /app

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libpaho-mqtt1.3 \
    libjson-c5 \
    libmsgpackc2 \
    libcbor0.8 \
    libprotobuf32 \
    libprotobuf-c1 \
    && rm -rf /var/lib/apt/lists/*

# Copy built binaries
COPY --from=builder /app/bin/ ./bin/

# Copy schemas
COPY schemas/ ./schemas/

# Create results directory
RUN mkdir -p results

# Set default command
CMD ["./bin/publisher"]

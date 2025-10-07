# C++ MQTT Implementation Dockerfile
FROM gcc:12 as builder

WORKDIR /app

# Install dependencies
RUN apt-get update && apt-get install -y \
    libpaho-mqtt-dev \
    libpaho-mqttpp-dev \
    libjsoncpp-dev \
    libmsgpack-dev \
    libcbor-dev \
    protobuf-compiler \
    libprotobuf-dev \
    cmake \
    make \
    && rm -rf /var/lib/apt/lists/*

# Copy C++ source code
COPY cpp/src/ ./src/
COPY cpp/CMakeLists.txt .
COPY schemas/ ./schemas/

# Generate protobuf files
RUN protoc --proto_path=./schemas --cpp_out=./src schemas/sensor_data.proto

# Build the project
RUN mkdir build && cd build && \
    cmake .. && \
    make

# Runtime stage
FROM debian:bookworm-slim

WORKDIR /app

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libpaho-mqtt1.3 \
    libpaho-mqttpp3-1 \
    libjsoncpp25 \
    libmsgpackc2 \
    libcbor0.8 \
    libprotobuf32 \
    && rm -rf /var/lib/apt/lists/*

# Copy built binaries
COPY --from=builder /app/build/bin/ ./bin/

# Copy schemas
COPY schemas/ ./schemas/

# Create results directory
RUN mkdir -p results

# Set default command
CMD ["./bin/publisher"]

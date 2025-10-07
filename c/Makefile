# C MQTT Implementation Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = -lpaho-mqtt3c -ljson-c -lpthread -lmsgpackc -lcbor -lprotobuf-c

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

# Source files
PUBLISHER_SRC = $(SRC_DIR)/publisher.c $(SRC_DIR)/sensor_data.c $(SRC_DIR)/json_encoder.c $(SRC_DIR)/sensor_data.pb-c.c
SUBSCRIBER_SRC = $(SRC_DIR)/subscriber.c $(SRC_DIR)/sensor_data.c $(SRC_DIR)/json_encoder.c $(SRC_DIR)/sensor_data.pb-c.c

# Object files
PUBLISHER_OBJ = $(PUBLISHER_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
SUBSCRIBER_OBJ = $(SUBSCRIBER_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Binaries
PUBLISHER_BIN = $(BIN_DIR)/publisher
SUBSCRIBER_BIN = $(BIN_DIR)/subscriber

.PHONY: all clean directories

all: directories $(PUBLISHER_BIN) $(SUBSCRIBER_BIN)

directories:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(PUBLISHER_BIN): $(PUBLISHER_OBJ)
	$(CC) $(PUBLISHER_OBJ) -o $@ $(LDFLAGS)

$(SUBSCRIBER_BIN): $(SUBSCRIBER_OBJ)
	$(CC) $(SUBSCRIBER_OBJ) -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

install-deps:
	@echo "Installing dependencies..."
	@echo "On Ubuntu/Debian: sudo apt-get install libpaho-mqtt-dev libjson-c-dev"
	@echo "On macOS: brew install paho-mqtt-c json-c"
	@echo "On CentOS/RHEL: sudo yum install paho-mqtt-c-devel json-c-devel"

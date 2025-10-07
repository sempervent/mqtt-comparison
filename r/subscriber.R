#!/usr/bin/env Rscript
# MQTT Subscriber for R with JSON encoding support

library(jsonlite)
library(mqtt)
library(msgpack)
library(cbor)
library(RProtoBuf)

# Parse command line arguments
parse_args <- function() {
  args <- commandArgs(trailingOnly = TRUE)
  
  # Set defaults
  broker <- "localhost"
  port <- 1883
  topic <- "mqtt-demo/all"
  qos <- 1
  encoding <- "json"
  
  # Parse arguments
  i <- 1
  while (i <= length(args)) {
    if (args[i] == "--broker" && i + 1 <= length(args)) {
      broker <- args[i + 1]
      i <- i + 2
    } else if (args[i] == "--port" && i + 1 <= length(args)) {
      port <- as.numeric(args[i + 1])
      i <- i + 2
    } else if (args[i] == "--topic" && i + 1 <= length(args)) {
      topic <- args[i + 1]
      i <- i + 2
    } else if (args[i] == "--qos" && i + 1 <= length(args)) {
      qos <- as.numeric(args[i + 1])
      i <- i + 2
    } else if (args[i] == "--encoding" && i + 1 <= length(args)) {
      encoding <- args[i + 1]
      i <- i + 2
    } else {
      i <- i + 1
    }
  }
  
  return(list(
    broker = broker,
    port = port,
    topic = topic,
    qos = qos,
    encoding = encoding
  ))
}

# Decode message
decode_message <- function(payload, encoding) {
  if (encoding == "json") {
    return(fromJSON(payload))
  } else if (encoding == "msgpack") {
    return(msgpack_unpack(payload))
  } else if (encoding == "cbor") {
    return(cbor_decode(payload))
  } else if (encoding == "protobuf") {
    # For protobuf, fallback to JSON for now
    return(fromJSON(payload))
  } else {
    stop(paste("Unsupported encoding:", encoding))
  }
}

# Global variables for message handling
message_count <- 0
receive_times <- numeric(0)

# Message callback function
message_callback <- function(topic, payload) {
  receive_time <- as.numeric(Sys.time())
  message_count <<- message_count + 1
  
  tryCatch({
    # Decode message
    data <- decode_message(payload, args$encoding)
    
    cat("\n[Message", message_count, "] Topic:", topic, "\n")
    cat("  Sensor ID:", data$sensor_id, "\n")
    cat("  Temperature:", round(data$temperature, 2), "°C\n")
    cat("  Humidity:", round(data$humidity, 2), "%\n")
    cat("  Pressure:", round(data$pressure, 2), " hPa\n")
    cat("  Timestamp:", round(data$timestamp, 6), "\n")
    
    # Calculate receive latency
    latency <- receive_time - data$timestamp
    receive_times <<- c(receive_times, latency)
    cat("  Receive latency:", round(latency * 1000, 2), "ms\n")
    
  }, error = function(e) {
    cat("\n[Message", message_count, "] Failed to decode message:", e$message, "\n")
  })
}

# Signal handler for cleanup
cleanup <- function() {
  cat("\n\n✓ Received", message_count, "messages\n")
  
  if (length(receive_times) > 0) {
    avg_latency <- mean(receive_times)
    cat("✓ Average receive latency:", round(avg_latency * 1000, 2), "ms\n")
  }
  
  cat("✓ Disconnected\n")
  quit(status = 0)
}

# Main function
main <- function() {
  args <- parse_args()
  
  cat("=== MQTT Subscriber (R) ===\n")
  cat("Broker:", args$broker, ":", args$port, "\n")
  cat("Topic:", args$topic, "\n")
  cat("QoS:", args$qos, "\n")
  cat("\n")
  
  # Set up signal handler
  on.exit(cleanup())
  
  tryCatch({
    # Create MQTT client
    client <- mqtt_client(args$broker, args$port)
    
    # Connect to broker
    mqtt_connect(client)
    cat("✓ Connected to", args$broker, ":", args$port, "\n")
    
    # Subscribe to topic
    mqtt_subscribe(client, args$topic, message_callback, qos = args$qos)
    cat("✓ Subscribed to topic:", args$topic, "(QoS:", args$qos, ")\n")
    cat("\nWaiting for messages (Ctrl+C to exit)...\n\n")
    
    # Keep running until interrupted
    while (TRUE) {
      Sys.sleep(0.1)
    }
    
  }, error = function(e) {
    cat("✗ Error:", e$message, "\n")
    quit(status = 1)
  })
}

# Run main function
main()

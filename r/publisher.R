#!/usr/bin/env Rscript
# MQTT Publisher for R with JSON encoding support

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
  sensor_id <- "sensor_001"
  count <- 10
  interval <- 1.0
  payload_size <- "small"
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
    } else if (args[i] == "--sensor-id" && i + 1 <= length(args)) {
      sensor_id <- args[i + 1]
      i <- i + 2
    } else if (args[i] == "--count" && i + 1 <= length(args)) {
      count <- as.numeric(args[i + 1])
      i <- i + 2
    } else if (args[i] == "--interval" && i + 1 <= length(args)) {
      interval <- as.numeric(args[i + 1])
      i <- i + 2
    } else if (args[i] == "--payload" && i + 1 <= length(args)) {
      payload_size <- args[i + 1]
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
    sensor_id = sensor_id,
    count = count,
    interval = interval,
    payload_size = payload_size,
    qos = qos,
    encoding = encoding
  ))
}

# Create sensor data
create_sensor_data <- function(sensor_id, payload_size) {
  # Set timestamp
  timestamp <- as.numeric(Sys.time())
  
  # Generate random values
  temperature <- 20.0 + runif(1, -5, 15)
  humidity <- 30.0 + runif(1, 0, 40)
  pressure <- 1000.0 + runif(1, -50, 50)
  
  # Create base data
  data <- list(
    timestamp = timestamp,
    sensor_id = sensor_id,
    temperature = temperature,
    humidity = humidity,
    pressure = pressure
  )
  
  # Add additional fields based on payload size
  if (payload_size == "medium") {
    data$location <- list(
      lat = 40.7128,
      lon = -74.0060,
      altitude = 10.5
    )
    data$status <- "active"
    data$battery_level <- 85.5
    data$signal_strength <- -65
    data$additional_data <- paste(rep("x", 1500), collapse = "")
  } else if (payload_size == "large") {
    data$location <- list(
      lat = 40.7128,
      lon = -74.0060,
      altitude = 10.5
    )
    data$status <- "active"
    data$battery_level <- 85.5
    data$signal_strength <- -65
    data$sensor_readings <- runif(100, 0, 100)
    data$metadata <- list(
      firmware_version = "1.2.3",
      hardware_id = "HW-001",
      calibration_date = "2024-01-01",
      last_maintenance = "2024-06-01"
    )
    data$additional_data <- paste(rep("x", 60000), collapse = "")
  }
  
  return(data)
}

# Encode message
encode_message <- function(data, encoding) {
  if (encoding == "json") {
    return(toJSON(data, auto_unbox = TRUE))
  } else if (encoding == "msgpack") {
    return(msgpack_pack(data))
  } else if (encoding == "cbor") {
    return(cbor_encode(data))
  } else if (encoding == "protobuf") {
    # For protobuf, fallback to JSON for now
    return(toJSON(data, auto_unbox = TRUE))
  } else {
    stop(paste("Unsupported encoding:", encoding))
  }
}

# Main function
main <- function() {
  args <- parse_args()
  
  cat("=== MQTT Publisher (R) ===\n")
  cat("Broker:", args$broker, ":", args$port, "\n")
  cat("Topic:", args$topic, "\n")
  cat("Payload:", args$payload_size, "\n")
  cat("QoS:", args$qos, "\n")
  cat("Encoding:", args$encoding, "\n")
  cat("\n")
  
  tryCatch({
    # Create MQTT client
    client <- mqtt_client(args$broker, args$port)
    
    # Connect to broker
    mqtt_connect(client)
    cat("✓ Connected to", args$broker, ":", args$port, "\n")
    
    # Publish messages
    publish_times <- numeric(args$count)
    
    for (i in 1:args$count) {
      cat("Publishing message", i, "/", args$count, "...\n")
      
      # Create sensor data
      data <- create_sensor_data(args$sensor_id, args$payload_size)
      
      # Measure publish time
      start_time <- Sys.time()
      
      # Encode message
      payload <- encode_message(data, args$encoding)
      
      # Publish message
      mqtt_publish(client, args$topic, payload, qos = args$qos)
      
      end_time <- Sys.time()
      publish_time <- as.numeric(end_time - start_time, units = "secs")
      publish_times[i] <- publish_time
      
      cat("  Publish time:", round(publish_time * 1000, 2), "ms\n")
      
      if (i < args$count) {
        Sys.sleep(args$interval)
      }
    }
    
    cat("\n")
    cat("✓ Published", args$count, "messages\n")
    
    if (length(publish_times) > 0) {
      avg_time <- mean(publish_times)
      cat("✓ Average publish time:", round(avg_time * 1000, 2), "ms\n")
    }
    
    # Disconnect
    mqtt_disconnect(client)
    cat("✓ Disconnected\n")
    
  }, error = function(e) {
    cat("✗ Error:", e$message, "\n")
    quit(status = 1)
  })
}

# Run main function
main()

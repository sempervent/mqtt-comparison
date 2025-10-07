#include "sensor_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simple JSON encoder for C implementation
// This is a basic implementation - in production, you'd use a proper JSON library

char* json_encode_sensor_data(const sensor_data_t *data) {
    if (!data) return NULL;
    
    // Calculate approximate size needed
    size_t buffer_size = 1024;
    if (data->additional_data) {
        buffer_size += data->additional_data_size;
    }
    if (data->sensor_readings) {
        buffer_size += data->sensor_readings_count * 20; // Approximate size per reading
    }
    
    char *json = malloc(buffer_size);
    if (!json) return NULL;
    
    // Build JSON string
    int len = snprintf(json, buffer_size,
        "{"
        "\"timestamp\":%.6f,"
        "\"sensor_id\":\"%s\","
        "\"temperature\":%.2f,"
        "\"humidity\":%.2f,"
        "\"pressure\":%.2f",
        data->timestamp,
        data->sensor_id,
        data->temperature,
        data->humidity,
        data->pressure
    );
    
    // Add optional fields
    if (strlen(data->status) > 0) {
        len += snprintf(json + len, buffer_size - len, ",\"status\":\"%s\"", data->status);
    }
    
    if (data->battery_level > 0) {
        len += snprintf(json + len, buffer_size - len, ",\"battery_level\":%.1f", data->battery_level);
    }
    
    if (data->signal_strength != 0) {
        len += snprintf(json + len, buffer_size - len, ",\"signal_strength\":%d", data->signal_strength);
    }
    
    // Add location
    if (data->location.lat != 0 || data->location.lon != 0) {
        len += snprintf(json + len, buffer_size - len,
            ",\"location\":{\"lat\":%.4f,\"lon\":%.4f,\"altitude\":%.1f}",
            data->location.lat, data->location.lon, data->location.altitude);
    }
    
    // Add sensor readings
    if (data->sensor_readings && data->sensor_readings_count > 0) {
        len += snprintf(json + len, buffer_size - len, ",\"sensor_readings\":[");
        for (size_t i = 0; i < data->sensor_readings_count; i++) {
            len += snprintf(json + len, buffer_size - len, "%.2f", data->sensor_readings[i]);
            if (i < data->sensor_readings_count - 1) {
                len += snprintf(json + len, buffer_size - len, ",");
            }
        }
        len += snprintf(json + len, buffer_size - len, "]");
    }
    
    // Add metadata
    if (strlen(data->metadata.firmware_version) > 0) {
        len += snprintf(json + len, buffer_size - len,
            ",\"metadata\":{"
            "\"firmware_version\":\"%s\","
            "\"hardware_id\":\"%s\","
            "\"calibration_date\":\"%s\","
            "\"last_maintenance\":\"%s\""
            "}",
            data->metadata.firmware_version,
            data->metadata.hardware_id,
            data->metadata.calibration_date,
            data->metadata.last_maintenance);
    }
    
    // Add additional data
    if (data->additional_data && data->additional_data_size > 0) {
        len += snprintf(json + len, buffer_size - len, ",\"additional_data\":\"%s\"", data->additional_data);
    }
    
    // Close JSON
    len += snprintf(json + len, buffer_size - len, "}");
    
    return json;
}

int json_decode_sensor_data(const char *json_str, sensor_data_t *data) {
    // This is a simplified decoder - in production, use a proper JSON library
    if (!json_str || !data) return -1;
    
    // For now, just parse basic fields using sscanf
    // This is a very basic implementation
    char sensor_id[64] = {0};
    double timestamp, temperature, humidity, pressure;
    
    int parsed = sscanf(json_str,
        "{\"timestamp\":%lf,\"sensor_id\":\"%63[^\"]\",\"temperature\":%lf,\"humidity\":%lf,\"pressure\":%lf",
        &timestamp, sensor_id, &temperature, &humidity, &pressure);
    
    if (parsed == 5) {
        data->timestamp = timestamp;
        strncpy(data->sensor_id, sensor_id, sizeof(data->sensor_id) - 1);
        data->temperature = temperature;
        data->humidity = humidity;
        data->pressure = pressure;
        return 0;
    }
    
    return -1;
}

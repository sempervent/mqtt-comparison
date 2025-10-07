#include "sensor_data.h"
#include <sstream>
#include <iomanip>

// JSON encoder for C++ implementation
// This is a basic implementation - in production, you'd use a proper JSON library

std::string json_encode_sensor_data(const SensorData& data) {
    return data.to_json();
}

bool json_decode_sensor_data(const std::string& json_str, SensorData& data) {
    return data.from_json(json_str);
}

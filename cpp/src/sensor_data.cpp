#include "sensor_data.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <random>
#include <msgpack.hpp>
#include <cbor.h>
#include "sensor_data.pb.h"

SensorData::SensorData(const std::string& sensor_id, const std::string& payload_size)
    : sensor_id(sensor_id), rng(std::random_device{}()) {
    
    // Set timestamp
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0;
    
    // Fill with random data
    fill_random();
    
    // Setup payload based on size
    setup_payload(payload_size);
}

void SensorData::fill_random() {
    std::uniform_real_distribution<double> temp_dist(-5.0, 15.0);
    std::uniform_real_distribution<double> humidity_dist(0.0, 40.0);
    std::uniform_real_distribution<double> pressure_dist(-50.0, 50.0);
    
    temperature = 20.0 + temp_dist(rng);
    humidity = 30.0 + humidity_dist(rng);
    pressure = 1000.0 + pressure_dist(rng);
}

void SensorData::setup_payload(const std::string& payload_size) {
    if (payload_size == "medium") {
        location = {40.7128, -74.0060, 10.5};
        status = "active";
        battery_level = 85.5;
        signal_strength = -65;
        additional_data = std::string(1500, 'x');
    } else if (payload_size == "large") {
        location = {40.7128, -74.0060, 10.5};
        status = "active";
        battery_level = 85.5;
        signal_strength = -65;
        
        // Add sensor readings
        std::uniform_real_distribution<double> reading_dist(0.0, 100.0);
        sensor_readings.resize(100);
        for (auto& reading : sensor_readings) {
            reading = reading_dist(rng);
        }
        
        // Add metadata
        metadata.firmware_version = "1.2.3";
        metadata.hardware_id = "HW-001";
        metadata.calibration_date = "2024-01-01";
        metadata.last_maintenance = "2024-06-01";
        
        // Add large padding data
        additional_data = std::string(60000, 'x');
    }
}

std::string SensorData::to_json() const {
    std::ostringstream json;
    json << std::fixed << std::setprecision(2);
    
    json << "{";
    json << "\"timestamp\":" << timestamp << ",";
    json << "\"sensor_id\":\"" << sensor_id << "\",";
    json << "\"temperature\":" << temperature << ",";
    json << "\"humidity\":" << humidity << ",";
    json << "\"pressure\":" << pressure;
    
    // Add optional fields
    if (!status.empty()) {
        json << ",\"status\":\"" << status << "\"";
    }
    
    if (battery_level > 0) {
        json << ",\"battery_level\":" << battery_level;
    }
    
    if (signal_strength != 0) {
        json << ",\"signal_strength\":" << signal_strength;
    }
    
    // Add location
    if (location.lat != 0 || location.lon != 0) {
        json << ",\"location\":{";
        json << "\"lat\":" << location.lat << ",";
        json << "\"lon\":" << location.lon << ",";
        json << "\"altitude\":" << location.altitude;
        json << "}";
    }
    
    // Add sensor readings
    if (!sensor_readings.empty()) {
        json << ",\"sensor_readings\":[";
        for (size_t i = 0; i < sensor_readings.size(); ++i) {
            if (i > 0) json << ",";
            json << sensor_readings[i];
        }
        json << "]";
    }
    
    // Add metadata
    if (!metadata.firmware_version.empty()) {
        json << ",\"metadata\":{";
        json << "\"firmware_version\":\"" << metadata.firmware_version << "\",";
        json << "\"hardware_id\":\"" << metadata.hardware_id << "\",";
        json << "\"calibration_date\":\"" << metadata.calibration_date << "\",";
        json << "\"last_maintenance\":\"" << metadata.last_maintenance << "\"";
        json << "}";
    }
    
    // Add additional data
    if (!additional_data.empty()) {
        json << ",\"additional_data\":\"" << additional_data << "\"";
    }
    
    json << "}";
    return json.str();
}

bool SensorData::from_json(const std::string& json_str) {
    // This is a simplified JSON parser - in production, use a proper JSON library
    // For now, just parse basic fields using string operations
    
    // Find timestamp
    size_t pos = json_str.find("\"timestamp\":");
    if (pos != std::string::npos) {
        pos += 11; // Skip "timestamp":
        size_t end = json_str.find_first_of(",}", pos);
        if (end != std::string::npos) {
            timestamp = std::stod(json_str.substr(pos, end - pos));
        }
    }
    
    // Find sensor_id
    pos = json_str.find("\"sensor_id\":\"");
    if (pos != std::string::npos) {
        pos += 12; // Skip "sensor_id":""
        size_t end = json_str.find("\"", pos);
        if (end != std::string::npos) {
            sensor_id = json_str.substr(pos, end - pos);
        }
    }
    
    // Find temperature
    pos = json_str.find("\"temperature\":");
    if (pos != std::string::npos) {
        pos += 13; // Skip "temperature":
        size_t end = json_str.find_first_of(",}", pos);
        if (end != std::string::npos) {
            temperature = std::stod(json_str.substr(pos, end - pos));
        }
    }
    
    // Find humidity
    pos = json_str.find("\"humidity\":");
    if (pos != std::string::npos) {
        pos += 10; // Skip "humidity":
        size_t end = json_str.find_first_of(",}", pos);
        if (end != std::string::npos) {
            humidity = std::stod(json_str.substr(pos, end - pos));
        }
    }
    
    // Find pressure
    pos = json_str.find("\"pressure\":");
    if (pos != std::string::npos) {
        pos += 10; // Skip "pressure":
        size_t end = json_str.find_first_of(",}", pos);
        if (end != std::string::npos) {
            pressure = std::stod(json_str.substr(pos, end - pos));
        }
    }
    
    return true;
}

std::vector<uint8_t> SensorData::to_msgpack() const {
    std::map<std::string, msgpack::object> map;
    map["timestamp"] = msgpack::object(timestamp);
    map["sensor_id"] = msgpack::object(sensor_id.c_str());
    map["temperature"] = msgpack::object(temperature);
    map["humidity"] = msgpack::object(humidity);
    map["pressure"] = msgpack::object(pressure);
    
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, map);
    
    std::vector<uint8_t> result(sbuf.size());
    std::memcpy(result.data(), sbuf.data(), sbuf.size());
    return result;
}

bool SensorData::from_msgpack(const std::vector<uint8_t>& msgpack_data) {
    try {
        msgpack::object_handle oh = msgpack::unpack(reinterpret_cast<const char*>(msgpack_data.data()), msgpack_data.size());
        msgpack::object obj = oh.get();
        
        if (obj.type != msgpack::type::MAP) {
            return false;
        }
        
        std::map<std::string, msgpack::object> map = obj.as<std::map<std::string, msgpack::object>>();
        
        if (map.find("timestamp") != map.end()) {
            timestamp = map["timestamp"].as<double>();
        }
        if (map.find("sensor_id") != map.end()) {
            sensor_id = map["sensor_id"].as<std::string>();
        }
        if (map.find("temperature") != map.end()) {
            temperature = map["temperature"].as<double>();
        }
        if (map.find("humidity") != map.end()) {
            humidity = map["humidity"].as<double>();
        }
        if (map.find("pressure") != map.end()) {
            pressure = map["pressure"].as<double>();
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<uint8_t> SensorData::to_cbor() const {
    cbor_item_t *map = cbor_new_definite_map(5);
    cbor_item_t *key, *value;

    // timestamp
    key = cbor_build_string("timestamp");
    value = cbor_build_float8(timestamp);
    cbor_map_add(map, (struct cbor_pair){.key = key, .value = value});

    // sensor_id
    key = cbor_build_string("sensor_id");
    value = cbor_build_string(sensor_id.c_str());
    cbor_map_add(map, (struct cbor_pair){.key = key, .value = value});

    // temperature
    key = cbor_build_string("temperature");
    value = cbor_build_float8(temperature);
    cbor_map_add(map, (struct cbor_pair){.key = key, .value = value});

    // humidity
    key = cbor_build_string("humidity");
    value = cbor_build_float8(humidity);
    cbor_map_add(map, (struct cbor_pair){.key = key, .value = value});

    // pressure
    key = cbor_build_string("pressure");
    value = cbor_build_float8(pressure);
    cbor_map_add(map, (struct cbor_pair){.key = key, .value = value});

    unsigned char *buffer;
    size_t buffer_size;
    cbor_serialize_alloc(map, &buffer, &buffer_size);

    std::vector<uint8_t> result(buffer, buffer + buffer_size);
    free(buffer);
    cbor_decref(&map);
    return result;
}

bool SensorData::from_cbor(const std::vector<uint8_t>& cbor_data) {
    struct cbor_load_result result;
    cbor_item_t *item = cbor_load(cbor_data.data(), cbor_data.size(), &result);
    if (result.error.code != CBOR_ERR_NONE) {
        return false;
    }

    if (!cbor_isa_map(item)) {
        cbor_decref(&item);
        return false;
    }

    size_t map_size = cbor_map_size(item);

    for (size_t i = 0; i < map_size; i++) {
        struct cbor_pair pair = cbor_map_handle(item)[i];
        if (cbor_isa_string(pair.key)) {
            char *key = (char*)cbor_string_handle(pair.key);
            size_t key_len = cbor_string_length(pair.key);

            if (strncmp(key, "timestamp", key_len) == 0 && cbor_is_float(pair.value)) {
                timestamp = cbor_float_get_float8(pair.value);
            } else if (strncmp(key, "sensor_id", key_len) == 0 && cbor_isa_string(pair.value)) {
                char *val = (char*)cbor_string_handle(pair.value);
                size_t val_len = cbor_string_length(pair.value);
                sensor_id = std::string(val, val_len);
            } else if (strncmp(key, "temperature", key_len) == 0 && cbor_is_float(pair.value)) {
                temperature = cbor_float_get_float8(pair.value);
            } else if (strncmp(key, "humidity", key_len) == 0 && cbor_is_float(pair.value)) {
                humidity = cbor_float_get_float8(pair.value);
            } else if (strncmp(key, "pressure", key_len) == 0 && cbor_is_float(pair.value)) {
                pressure = cbor_float_get_float8(pair.value);
            }
        }
    }

    cbor_decref(&item);
    return true;
}

std::vector<uint8_t> SensorData::to_protobuf() const {
    mqtt_comparison::SensorData msg;
    msg.set_timestamp(timestamp);
    msg.set_sensor_id(sensor_id);
    msg.set_temperature(temperature);
    msg.set_humidity(humidity);
    msg.set_pressure(pressure);

    std::string serialized;
    msg.SerializeToString(&serialized);
    return std::vector<uint8_t>(serialized.begin(), serialized.end());
}

bool SensorData::from_protobuf(const std::vector<uint8_t>& protobuf_data) {
    mqtt_comparison::SensorData msg;
    if (!msg.ParseFromArray(protobuf_data.data(), protobuf_data.size())) {
        return false;
    }

    timestamp = msg.timestamp();
    sensor_id = msg.sensor_id();
    temperature = msg.temperature();
    humidity = msg.humidity();
    pressure = msg.pressure();

    return true;
}

size_t SensorData::get_size() const {
    size_t size = sizeof(SensorData);
    size += sensor_id.size();
    size += status.size();
    size += sensor_readings.size() * sizeof(double);
    size += metadata.firmware_version.size();
    size += metadata.hardware_id.size();
    size += metadata.calibration_date.size();
    size += metadata.last_maintenance.size();
    size += additional_data.size();
    return size;
}

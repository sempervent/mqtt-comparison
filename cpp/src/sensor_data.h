#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <string>
#include <vector>
#include <chrono>
#include <random>

struct Location {
    double lat;
    double lon;
    double altitude;
};

struct Metadata {
    std::string firmware_version;
    std::string hardware_id;
    std::string calibration_date;
    std::string last_maintenance;
};

class SensorData {
public:
    double timestamp;
    std::string sensor_id;
    double temperature;
    double humidity;
    double pressure;
    
    // Optional fields for larger payloads
    Location location;
    std::string status;
    double battery_level;
    int32_t signal_strength;
    std::vector<double> sensor_readings;
    Metadata metadata;
    std::string additional_data;
    
    // Constructors
    SensorData() = default;
    SensorData(const std::string& sensor_id, const std::string& payload_size = "small");
    
    // Destructor
    ~SensorData() = default;
    
    // Methods
    void fill_random();
    std::string to_json() const;
    bool from_json(const std::string& json_str);
    std::vector<uint8_t> to_msgpack() const;
    bool from_msgpack(const std::vector<uint8_t>& msgpack_data);
    std::vector<uint8_t> to_cbor() const;
    bool from_cbor(const std::vector<uint8_t>& cbor_data);
    std::vector<uint8_t> to_protobuf() const;
    bool from_protobuf(const std::vector<uint8_t>& protobuf_data);
    size_t get_size() const;
    
private:
    std::mt19937 rng;
    void setup_payload(const std::string& payload_size);
};

#endif // SENSOR_DATA_H

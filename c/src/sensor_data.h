#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <stdint.h>
#include <time.h>
#include <msgpack.h>

typedef struct {
    double timestamp;
    char sensor_id[64];
    double temperature;
    double humidity;
    double pressure;
    
    // Optional fields for larger payloads
    struct {
        double lat;
        double lon;
        double altitude;
    } location;
    
    char status[32];
    double battery_level;
    int32_t signal_strength;
    double *sensor_readings;
    size_t sensor_readings_count;
    
    struct {
        char firmware_version[32];
        char hardware_id[32];
        char calibration_date[32];
        char last_maintenance[32];
    } metadata;
    
    char *additional_data;
    size_t additional_data_size;
} sensor_data_t;

// Function declarations
sensor_data_t* sensor_data_create(const char *sensor_id, const char *payload_size);
void sensor_data_destroy(sensor_data_t *data);
void sensor_data_fill_random(sensor_data_t *data);
char* sensor_data_to_json(const sensor_data_t *data);
int sensor_data_from_json(const char *json_str, sensor_data_t *data);
char* sensor_data_to_msgpack(const sensor_data_t *data);
int sensor_data_from_msgpack(const char *msgpack_str, size_t len, sensor_data_t *data);
char* sensor_data_to_cbor(const sensor_data_t *data);
int sensor_data_from_cbor(const char *cbor_str, size_t len, sensor_data_t *data);
char* sensor_data_to_protobuf(const sensor_data_t *data);
int sensor_data_from_protobuf(const char *protobuf_str, size_t len, sensor_data_t *data);
size_t sensor_data_get_size(const sensor_data_t *data);

#endif // SENSOR_DATA_H

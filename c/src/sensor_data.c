#include "sensor_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <json-c/json.h>
#include <msgpack.h>
#include <cbor.h>
#include "sensor_data.pb-c.h"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

sensor_data_t* sensor_data_create(const char *sensor_id, const char *payload_size) {
    sensor_data_t *data = calloc(1, sizeof(sensor_data_t));
    if (!data) return NULL;
    
    // Set basic fields
    data->timestamp = time(NULL);
    strncpy(data->sensor_id, sensor_id, sizeof(data->sensor_id) - 1);
    
    // Fill with random data
    sensor_data_fill_random(data);
    
    // Add additional fields based on payload size
    if (strcmp(payload_size, "medium") == 0) {
        data->location.lat = 40.7128;
        data->location.lon = -74.0060;
        data->location.altitude = 10.5;
        strcpy(data->status, "active");
        data->battery_level = 85.5;
        data->signal_strength = -65;
        
        // Add padding data
        data->additional_data_size = 1500;
        data->additional_data = malloc(data->additional_data_size);
        if (data->additional_data) {
            memset(data->additional_data, 'x', data->additional_data_size - 1);
            data->additional_data[data->additional_data_size - 1] = '\0';
        }
    } else if (strcmp(payload_size, "large") == 0) {
        data->location.lat = 40.7128;
        data->location.lon = -74.0060;
        data->location.altitude = 10.5;
        strcpy(data->status, "active");
        data->battery_level = 85.5;
        data->signal_strength = -65;
        
        // Add sensor readings array
        data->sensor_readings_count = 100;
        data->sensor_readings = malloc(data->sensor_readings_count * sizeof(double));
        if (data->sensor_readings) {
            for (size_t i = 0; i < data->sensor_readings_count; i++) {
                data->sensor_readings[i] = ((double)rand() / RAND_MAX) * 100.0;
            }
        }
        
        // Add metadata
        strcpy(data->metadata.firmware_version, "1.2.3");
        strcpy(data->metadata.hardware_id, "HW-001");
        strcpy(data->metadata.calibration_date, "2024-01-01");
        strcpy(data->metadata.last_maintenance, "2024-06-01");
        
        // Add large padding data
        data->additional_data_size = 60000;
        data->additional_data = malloc(data->additional_data_size);
        if (data->additional_data) {
            memset(data->additional_data, 'x', data->additional_data_size - 1);
            data->additional_data[data->additional_data_size - 1] = '\0';
        }
    }
    
    return data;
}

void sensor_data_destroy(sensor_data_t *data) {
    if (!data) return;
    
    if (data->sensor_readings) {
        free(data->sensor_readings);
    }
    if (data->additional_data) {
        free(data->additional_data);
    }
    free(data);
}

void sensor_data_fill_random(sensor_data_t *data) {
    if (!data) return;
    
    // Generate random values
    data->temperature = 20.0 + ((double)rand() / RAND_MAX) * 20.0 - 5.0;
    data->humidity = 30.0 + ((double)rand() / RAND_MAX) * 40.0;
    data->pressure = 1000.0 + ((double)rand() / RAND_MAX) * 100.0 - 50.0;
}

char* sensor_data_to_json(const sensor_data_t *data) {
    if (!data) return NULL;
    
    json_object *root = json_object_new_object();
    if (!root) return NULL;
    
    // Add basic fields
    json_object_object_add(root, "timestamp", json_object_new_double(data->timestamp));
    json_object_object_add(root, "sensor_id", json_object_new_string(data->sensor_id));
    json_object_object_add(root, "temperature", json_object_new_double(data->temperature));
    json_object_object_add(root, "humidity", json_object_new_double(data->humidity));
    json_object_object_add(root, "pressure", json_object_new_double(data->pressure));
    
    // Add optional fields if they exist
    if (strlen(data->status) > 0) {
        json_object_object_add(root, "status", json_object_new_string(data->status));
    }
    
    if (data->battery_level > 0) {
        json_object_object_add(root, "battery_level", json_object_new_double(data->battery_level));
    }
    
    if (data->signal_strength != 0) {
        json_object_object_add(root, "signal_strength", json_object_new_int(data->signal_strength));
    }
    
    // Add location if present
    if (data->location.lat != 0 || data->location.lon != 0) {
        json_object *location = json_object_new_object();
        json_object_object_add(location, "lat", json_object_new_double(data->location.lat));
        json_object_object_add(location, "lon", json_object_new_double(data->location.lon));
        json_object_object_add(location, "altitude", json_object_new_double(data->location.altitude));
        json_object_object_add(root, "location", location);
    }
    
    // Add sensor readings if present
    if (data->sensor_readings && data->sensor_readings_count > 0) {
        json_object *readings = json_object_new_array();
        for (size_t i = 0; i < data->sensor_readings_count; i++) {
            json_object_array_add(readings, json_object_new_double(data->sensor_readings[i]));
        }
        json_object_object_add(root, "sensor_readings", readings);
    }
    
    // Add metadata if present
    if (strlen(data->metadata.firmware_version) > 0) {
        json_object *metadata = json_object_new_object();
        json_object_object_add(metadata, "firmware_version", json_object_new_string(data->metadata.firmware_version));
        json_object_object_add(metadata, "hardware_id", json_object_new_string(data->metadata.hardware_id));
        json_object_object_add(metadata, "calibration_date", json_object_new_string(data->metadata.calibration_date));
        json_object_object_add(metadata, "last_maintenance", json_object_new_string(data->metadata.last_maintenance));
        json_object_object_add(root, "metadata", metadata);
    }
    
    // Add additional data if present
    if (data->additional_data && data->additional_data_size > 0) {
        json_object_object_add(root, "additional_data", json_object_new_string(data->additional_data));
    }
    
    const char *json_string = json_object_to_json_string(root);
    char *result = malloc(strlen(json_string) + 1);
    if (result) {
        strcpy(result, json_string);
    }
    
    json_object_put(root);
    return result;
}

int sensor_data_from_json(const char *json_str, sensor_data_t *data) {
    if (!json_str || !data) return -1;
    
    json_object *root = json_tokener_parse(json_str);
    if (!root) return -1;
    
    // Parse basic fields
    json_object *obj;
    if (json_object_object_get_ex(root, "timestamp", &obj)) {
        data->timestamp = json_object_get_double(obj);
    }
    if (json_object_object_get_ex(root, "sensor_id", &obj)) {
        strncpy(data->sensor_id, json_object_get_string(obj), sizeof(data->sensor_id) - 1);
    }
    if (json_object_object_get_ex(root, "temperature", &obj)) {
        data->temperature = json_object_get_double(obj);
    }
    if (json_object_object_get_ex(root, "humidity", &obj)) {
        data->humidity = json_object_get_double(obj);
    }
    if (json_object_object_get_ex(root, "pressure", &obj)) {
        data->pressure = json_object_get_double(obj);
    }
    
    // Parse optional fields
    if (json_object_object_get_ex(root, "status", &obj)) {
        strncpy(data->status, json_object_get_string(obj), sizeof(data->status) - 1);
    }
    if (json_object_object_get_ex(root, "battery_level", &obj)) {
        data->battery_level = json_object_get_double(obj);
    }
    if (json_object_object_get_ex(root, "signal_strength", &obj)) {
        data->signal_strength = json_object_get_int(obj);
    }
    
    json_object_put(root);
    return 0;
}

char* sensor_data_to_msgpack(const sensor_data_t *data) {
    if (!data) return NULL;
    
    msgpack_sbuffer sbuf;
    msgpack_packer pk;
    msgpack_sbuffer_init(&sbuf);
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
    
    // Pack as map
    msgpack_pack_map(&pk, 5); // Basic 5 fields
    
    // Pack timestamp
    msgpack_pack_str(&pk, 9);
    msgpack_pack_str_body(&pk, "timestamp", 9);
    msgpack_pack_double(&pk, data->timestamp);
    
    // Pack sensor_id
    msgpack_pack_str(&pk, 9);
    msgpack_pack_str_body(&pk, "sensor_id", 9);
    msgpack_pack_str(&pk, strlen(data->sensor_id));
    msgpack_pack_str_body(&pk, data->sensor_id, strlen(data->sensor_id));
    
    // Pack temperature
    msgpack_pack_str(&pk, 11);
    msgpack_pack_str_body(&pk, "temperature", 11);
    msgpack_pack_double(&pk, data->temperature);
    
    // Pack humidity
    msgpack_pack_str(&pk, 8);
    msgpack_pack_str_body(&pk, "humidity", 8);
    msgpack_pack_double(&pk, data->humidity);
    
    // Pack pressure
    msgpack_pack_str(&pk, 8);
    msgpack_pack_str_body(&pk, "pressure", 8);
    msgpack_pack_double(&pk, data->pressure);
    
    char *result = malloc(sbuf.size);
    if (result) {
        memcpy(result, sbuf.data, sbuf.size);
    }
    
    msgpack_sbuffer_destroy(&sbuf);
    return result;
}

int sensor_data_from_msgpack(const char *msgpack_str, size_t len, sensor_data_t *data) {
    if (!msgpack_str || !data) return -1;
    
    msgpack_unpacked unpacked;
    msgpack_unpacked_init(&unpacked);
    
    size_t offset = 0;
    if (msgpack_unpack_next(&unpacked, msgpack_str, len, &offset) != MSGPACK_UNPACK_SUCCESS) {
        msgpack_unpacked_destroy(&unpacked);
        return -1;
    }
    
    msgpack_object obj = unpacked.data;
    if (obj.type != MSGPACK_OBJECT_MAP) {
        msgpack_unpacked_destroy(&unpacked);
        return -1;
    }
    
    // Parse map fields
    for (uint32_t i = 0; i < obj.via.map.size; i++) {
        msgpack_object_kv *kv = &obj.via.map.ptr[i];
        
        if (kv->key.type == MSGPACK_OBJECT_STR) {
            const char *key = kv->key.via.str.ptr;
            size_t key_len = kv->key.via.str.size;
            
            if (strncmp(key, "timestamp", key_len) == 0 && kv->val.type == MSGPACK_OBJECT_FLOAT) {
                data->timestamp = kv->val.via.f64;
            } else if (strncmp(key, "sensor_id", key_len) == 0 && kv->val.type == MSGPACK_OBJECT_STR) {
                strncpy(data->sensor_id, kv->val.via.str.ptr, 
                       MIN(kv->val.via.str.size, sizeof(data->sensor_id) - 1));
            } else if (strncmp(key, "temperature", key_len) == 0 && kv->val.type == MSGPACK_OBJECT_FLOAT) {
                data->temperature = kv->val.via.f64;
            } else if (strncmp(key, "humidity", key_len) == 0 && kv->val.type == MSGPACK_OBJECT_FLOAT) {
                data->humidity = kv->val.via.f64;
            } else if (strncmp(key, "pressure", key_len) == 0 && kv->val.type == MSGPACK_OBJECT_FLOAT) {
                data->pressure = kv->val.via.f64;
            }
        }
    }
    
    msgpack_unpacked_destroy(&unpacked);
    return 0;
}

// CBOR encoding functions
char* sensor_data_to_cbor(const sensor_data_t *data) {
    if (!data) return NULL;
    
    cbor_item_t *map = cbor_new_definite_map(5);
    cbor_item_t *key, *value;
    
    // timestamp
    key = cbor_build_string("timestamp");
    value = cbor_build_float8(data->timestamp);
    cbor_map_add(map, (struct cbor_pair){.key = key, .value = value});
    
    // sensor_id
    key = cbor_build_string("sensor_id");
    value = cbor_build_string(data->sensor_id);
    cbor_map_add(map, (struct cbor_pair){.key = key, .value = value});
    
    // temperature
    key = cbor_build_string("temperature");
    value = cbor_build_float8(data->temperature);
    cbor_map_add(map, (struct cbor_pair){.key = key, .value = value});
    
    // humidity
    key = cbor_build_string("humidity");
    value = cbor_build_float8(data->humidity);
    cbor_map_add(map, (struct cbor_pair){.key = key, .value = value});
    
    // pressure
    key = cbor_build_string("pressure");
    value = cbor_build_float8(data->pressure);
    cbor_map_add(map, (struct cbor_pair){.key = key, .value = value});
    
    unsigned char *buffer;
    size_t buffer_size;
    cbor_serialize_alloc(map, &buffer, &buffer_size);
    
    char *result = malloc(buffer_size);
    if (result) {
        memcpy(result, buffer, buffer_size);
    }
    
    free(buffer);
    cbor_decref(&map);
    return result;
}

int sensor_data_from_cbor(const char *cbor_str, size_t len, sensor_data_t *data) {
    if (!cbor_str || !data) return -1;
    
    struct cbor_load_result result;
    cbor_item_t *item = cbor_load((unsigned char*)cbor_str, len, &result);
    if (result.error.code != CBOR_ERR_NONE) {
        return -1;
    }
    
    if (!cbor_isa_map(item)) {
        cbor_decref(&item);
        return -1;
    }
    
    cbor_item_t *map = cbor_map_handle(item);
    size_t map_size = cbor_map_size(map);
    
    for (size_t i = 0; i < map_size; i++) {
        struct cbor_pair pair = cbor_map_handle(map)[i];
        if (cbor_isa_string(pair.key)) {
            char *key = (char*)cbor_string_handle(pair.key);
            size_t key_len = cbor_string_length(pair.key);
            
            if (strncmp(key, "timestamp", key_len) == 0 && cbor_is_float(pair.value)) {
                data->timestamp = cbor_float_get_float8(pair.value);
            } else if (strncmp(key, "sensor_id", key_len) == 0 && cbor_isa_string(pair.value)) {
                char *val = (char*)cbor_string_handle(pair.value);
                size_t val_len = cbor_string_length(pair.value);
                strncpy(data->sensor_id, val, MIN(val_len, sizeof(data->sensor_id) - 1));
            } else if (strncmp(key, "temperature", key_len) == 0 && cbor_is_float(pair.value)) {
                data->temperature = cbor_float_get_float8(pair.value);
            } else if (strncmp(key, "humidity", key_len) == 0 && cbor_is_float(pair.value)) {
                data->humidity = cbor_float_get_float8(pair.value);
            } else if (strncmp(key, "pressure", key_len) == 0 && cbor_is_float(pair.value)) {
                data->pressure = cbor_float_get_float8(pair.value);
            }
        }
    }
    
    cbor_decref(&item);
    return 0;
}

// Protobuf encoding functions
char* sensor_data_to_protobuf(const sensor_data_t *data) {
    if (!data) return NULL;

    MqttComparison__SensorData msg = MQTT_COMPARISON__SENSOR_DATA__INIT;
    msg.timestamp = data->timestamp;
    msg.sensor_id = (char*)data->sensor_id;
    msg.temperature = data->temperature;
    msg.humidity = data->humidity;
    msg.pressure = data->pressure;

    size_t len = mqtt_comparison__sensor_data__get_packed_size(&msg);
    char *buffer = malloc(len);
    if (buffer) {
        mqtt_comparison__sensor_data__pack(&msg, (uint8_t*)buffer);
    }

    return buffer;
}

int sensor_data_from_protobuf(const char *protobuf_str, size_t len, sensor_data_t *data) {
    if (!protobuf_str || !data) return -1;
    
    MqttComparison__SensorData *msg = mqtt_comparison__sensor_data__unpack(NULL, len, (uint8_t*)protobuf_str);
    if (!msg) return -1;

    data->timestamp = msg->timestamp;
    strncpy(data->sensor_id, msg->sensor_id, sizeof(data->sensor_id) - 1);
    data->temperature = msg->temperature;
    data->humidity = msg->humidity;
    data->pressure = msg->pressure;

    mqtt_comparison__sensor_data__free_unpacked(msg, NULL);
    return 0;
}

size_t sensor_data_get_size(const sensor_data_t *data) {
    if (!data) return 0;
    
    size_t size = sizeof(sensor_data_t);
    if (data->sensor_readings) {
        size += data->sensor_readings_count * sizeof(double);
    }
    if (data->additional_data) {
        size += data->additional_data_size;
    }
    return size;
}

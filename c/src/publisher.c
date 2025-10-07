#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <MQTTClient.h>
#include "sensor_data.h"

#define ADDRESS "tcp://localhost:1883"
#define CLIENTID "c_publisher"
#define QOS 1
#define TIMEOUT 10000L

typedef struct {
    char broker[256];
    int port;
    char topic[256];
    char sensor_id[64];
    int count;
    double interval;
    char payload_size[32];
    int qos;
    char encoding[32];
} publisher_args_t;

void parse_args(int argc, char *argv[], publisher_args_t *args) {
    // Set defaults from environment variables
    const char *env_broker = getenv("MQTT_BROKER");
    const char *env_port = getenv("MQTT_PORT");
    
    strcpy(args->broker, env_broker ? env_broker : "localhost");
    args->port = env_port ? atoi(env_port) : 1883;
    strcpy(args->topic, "mqtt-demo/all");
    strcpy(args->sensor_id, "sensor_001");
    args->count = 10;
    args->interval = 1.0;
    strcpy(args->payload_size, "small");
    args->qos = 1;
    strcpy(args->encoding, "json");
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--broker") == 0 && i + 1 < argc) {
            strncpy(args->broker, argv[++i], sizeof(args->broker) - 1);
        } else if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            args->port = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--topic") == 0 && i + 1 < argc) {
            strncpy(args->topic, argv[++i], sizeof(args->topic) - 1);
        } else if (strcmp(argv[i], "--sensor-id") == 0 && i + 1 < argc) {
            strncpy(args->sensor_id, argv[++i], sizeof(args->sensor_id) - 1);
        } else if (strcmp(argv[i], "--count") == 0 && i + 1 < argc) {
            args->count = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--interval") == 0 && i + 1 < argc) {
            args->interval = atof(argv[++i]);
        } else if (strcmp(argv[i], "--payload") == 0 && i + 1 < argc) {
            strncpy(args->payload_size, argv[++i], sizeof(args->payload_size) - 1);
        } else if (strcmp(argv[i], "--qos") == 0 && i + 1 < argc) {
            args->qos = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--encoding") == 0 && i + 1 < argc) {
            strncpy(args->encoding, argv[++i], sizeof(args->encoding) - 1);
        }
    }
}

void delivery_complete(void *context, MQTTClient_deliveryToken token) {
    printf("  Message %d delivered\n", token);
}

int main(int argc, char *argv[]) {
    publisher_args_t args;
    parse_args(argc, argv, &args);
    
    printf("=== MQTT Publisher (C) ===\n");
    printf("Broker: %s:%d\n", args.broker, args.port);
    printf("Topic: %s\n", args.topic);
    printf("Payload: %s\n", args.payload_size);
    printf("QoS: %d\n", args.qos);
    printf("\n");
    
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;
    
    // Create MQTT client
    char address[512];
    snprintf(address, sizeof(address), "tcp://%s:%d", args.broker, args.port);
    
    rc = MQTTClient_create(&client, address, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to create client, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    
    // Set delivery callback
    MQTTClient_setCallbacks(client, NULL, NULL, delivery_complete, NULL);
    
    // Connect to broker
    conn_opts.keepAliveInterval = 60;
    conn_opts.cleansession = 1;
    
    rc = MQTTClient_connect(client, &conn_opts);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        MQTTClient_destroy(&client);
        exit(EXIT_FAILURE);
    }
    
    printf("✓ Connected to %s:%d\n", args.broker, args.port);
    
    // Publish messages
    double *publish_times = malloc(args.count * sizeof(double));
    if (!publish_times) {
        printf("Failed to allocate memory for timing data\n");
        MQTTClient_disconnect(client, 10000);
        MQTTClient_destroy(&client);
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < args.count; i++) {
        printf("Publishing message %d/%d...\n", i + 1, args.count);
        
        // Create sensor data
        sensor_data_t *data = sensor_data_create(args.sensor_id, args.payload_size);
        if (!data) {
            printf("Failed to create sensor data\n");
            continue;
        }
        
        // Encode message based on encoding type
        char *payload = NULL;
        int payload_len = 0;
        
        if (strcmp(args.encoding, "json") == 0) {
            payload = sensor_data_to_json(data);
            if (payload) payload_len = strlen(payload);
        } else if (strcmp(args.encoding, "msgpack") == 0) {
            payload = sensor_data_to_msgpack(data);
            if (payload) payload_len = strlen(payload);
        } else if (strcmp(args.encoding, "cbor") == 0) {
            payload = sensor_data_to_cbor(data);
            if (payload) payload_len = strlen(payload);
        } else if (strcmp(args.encoding, "protobuf") == 0) {
            payload = sensor_data_to_protobuf(data);
            if (payload) payload_len = strlen(payload);
        } else {
            printf("Unsupported encoding: %s\n", args.encoding);
            sensor_data_destroy(data);
            continue;
        }
        
        if (!payload) {
            printf("Failed to encode sensor data\n");
            sensor_data_destroy(data);
            continue;
        }
        
        // Measure publish time
        clock_t start = clock();
        
        // Publish message
        pubmsg.payload = payload;
        pubmsg.payloadlen = payload_len;
        pubmsg.qos = args.qos;
        pubmsg.retained = 0;
        
        rc = MQTTClient_publishMessage(client, args.topic, &pubmsg, &token);
        if (rc != MQTTCLIENT_SUCCESS) {
            printf("Failed to publish message, return code %d\n", rc);
        } else {
            rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
            if (rc != MQTTCLIENT_SUCCESS) {
                printf("Failed to wait for completion, return code %d\n", rc);
            }
        }
        
        clock_t end = clock();
        double publish_time = ((double)(end - start)) / CLOCKS_PER_SEC;
        publish_times[i] = publish_time;
        printf("  Publish time: %.2fms\n", publish_time * 1000.0);
        
        // Cleanup
        free(payload);
        sensor_data_destroy(data);
        
        if (i < args.count - 1) {
            usleep((unsigned int)(args.interval * 1000000));
        }
    }
    
    printf("\n✓ Published %d messages\n", args.count);
    
    // Calculate average publish time
    double total_time = 0.0;
    for (int i = 0; i < args.count; i++) {
        total_time += publish_times[i];
    }
    double avg_time = total_time / args.count;
    printf("✓ Average publish time: %.2fms\n", avg_time * 1000.0);
    
    // Cleanup
    free(publish_times);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    
    return 0;
}

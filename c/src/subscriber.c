#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <MQTTClient.h>
#include "sensor_data.h"

#define ADDRESS "tcp://localhost:1883"
#define CLIENTID "c_subscriber"
#define QOS 1
#define TIMEOUT 10000L

typedef struct {
    char broker[256];
    int port;
    char topic[256];
    int qos;
    int message_count;
    double *receive_times;
    int max_messages;
} subscriber_context_t;

static subscriber_context_t *g_context = NULL;

void parse_args(int argc, char *argv[], subscriber_context_t *ctx) {
    // Set defaults
    strcpy(ctx->broker, "localhost");
    ctx->port = 1883;
    strcpy(ctx->topic, "mqtt-demo/all");
    ctx->qos = 1;
    ctx->message_count = 0;
    ctx->max_messages = 1000; // Reasonable limit
    ctx->receive_times = NULL;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--broker") == 0 && i + 1 < argc) {
            strncpy(ctx->broker, argv[++i], sizeof(ctx->broker) - 1);
        } else if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            ctx->port = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--topic") == 0 && i + 1 < argc) {
            strncpy(ctx->topic, argv[++i], sizeof(ctx->topic) - 1);
        } else if (strcmp(argv[i], "--qos") == 0 && i + 1 < argc) {
            ctx->qos = atoi(argv[++i]);
        }
    }
}

int message_arrived(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    subscriber_context_t *ctx = (subscriber_context_t *)context;
    double receive_time = time(NULL);
    
    printf("\n[Message %d] Topic: %s\n", ctx->message_count + 1, topicName);
    
    // Decode message
    char *payload = malloc(message->payloadlen + 1);
    if (payload) {
        memcpy(payload, message->payload, message->payloadlen);
        payload[message->payloadlen] = '\0';
        
        sensor_data_t data = {0};
        if (json_decode_sensor_data(payload, &data) == 0) {
            printf("  Sensor ID: %s\n", data.sensor_id);
            printf("  Temperature: %.2f°C\n", data.temperature);
            printf("  Humidity: %.2f%%\n", data.humidity);
            printf("  Pressure: %.2f hPa\n", data.pressure);
            printf("  Timestamp: %.6f\n", data.timestamp);
            
            // Calculate receive latency
            double latency = receive_time - data.timestamp;
            if (ctx->receive_times && ctx->message_count < ctx->max_messages) {
                ctx->receive_times[ctx->message_count] = latency;
            }
            printf("  Receive latency: %.2fms\n", latency * 1000.0);
        } else {
            printf("  Failed to decode message\n");
        }
        
        free(payload);
    }
    
    ctx->message_count++;
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    
    return 1;
}

void connection_lost(void *context, char *cause) {
    printf("Connection lost: %s\n", cause);
}

void delivery_complete(void *context, MQTTClient_deliveryToken token) {
    // Not used in subscriber
}

void signal_handler(int sig) {
    if (g_context) {
        printf("\n\n✓ Received %d messages\n", g_context->message_count);
        
        if (g_context->receive_times && g_context->message_count > 0) {
            double total_latency = 0.0;
            int valid_times = 0;
            for (int i = 0; i < g_context->message_count && i < g_context->max_messages; i++) {
                if (g_context->receive_times[i] > 0) {
                    total_latency += g_context->receive_times[i];
                    valid_times++;
                }
            }
            if (valid_times > 0) {
                double avg_latency = total_latency / valid_times;
                printf("✓ Average receive latency: %.2fms\n", avg_latency * 1000.0);
            }
        }
        
        printf("✓ Disconnected\n");
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    subscriber_context_t context = {0};
    parse_args(argc, argv, &context);
    g_context = &context;
    
    printf("=== MQTT Subscriber (C) ===\n");
    printf("Broker: %s:%d\n", context.broker, context.port);
    printf("Topic: %s\n", context.topic);
    printf("QoS: %d\n", context.qos);
    printf("\n");
    
    // Allocate memory for timing data
    context.receive_times = malloc(context.max_messages * sizeof(double));
    if (!context.receive_times) {
        printf("Failed to allocate memory for timing data\n");
        exit(EXIT_FAILURE);
    }
    
    // Set up signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    
    // Create MQTT client
    char address[512];
    snprintf(address, sizeof(address), "tcp://%s:%d", context.broker, context.port);
    
    rc = MQTTClient_create(&client, address, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to create client, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    
    // Set callbacks
    MQTTClient_setCallbacks(client, &context, connection_lost, message_arrived, delivery_complete);
    
    // Connect to broker
    conn_opts.keepAliveInterval = 60;
    conn_opts.cleansession = 1;
    
    rc = MQTTClient_connect(client, &conn_opts);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        MQTTClient_destroy(&client);
        exit(EXIT_FAILURE);
    }
    
    printf("✓ Connected to %s:%d\n", context.broker, context.port);
    printf("✓ Subscribed to topic: %s (QoS: %d)\n", context.topic, context.qos);
    printf("\nWaiting for messages (Ctrl+C to exit)...\n\n");
    
    // Subscribe to topic
    rc = MQTTClient_subscribe(client, context.topic, context.qos);
    if (rc != MQTTCLIENT_SUCCESS) {
        printf("Failed to subscribe, return code %d\n", rc);
        MQTTClient_disconnect(client, 10000);
        MQTTClient_destroy(&client);
        exit(EXIT_FAILURE);
    }
    
    // Keep running until interrupted
    while (1) {
        usleep(100000); // 100ms
    }
    
    // Cleanup (this won't be reached due to signal handler)
    free(context.receive_times);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    
    return 0;
}

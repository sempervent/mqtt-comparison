#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>
#include <signal.h>
#include <mqtt/async_client.h>
#include "sensor_data.h"

class SubscriberArgs {
public:
    std::string broker = "localhost";
    int port = 1883;
    std::string topic = "mqtt-demo/all";
    int qos = 1;
    std::string encoding = "json";
    
    void parse_args(int argc, char* argv[]) {
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "--broker" && i + 1 < argc) {
                broker = argv[++i];
            } else if (arg == "--port" && i + 1 < argc) {
                port = std::stoi(argv[++i]);
            } else if (arg == "--topic" && i + 1 < argc) {
                topic = argv[++i];
            } else if (arg == "--qos" && i + 1 < argc) {
                qos = std::stoi(argv[++i]);
            } else if (arg == "--encoding" && i + 1 < argc) {
                encoding = argv[++i];
            }
        }
    }
};

class SensorDataSubscriber {
private:
    mqtt::async_client client;
    std::string topic;
    int qos;
    std::string encoding;
    int message_count;
    std::vector<double> receive_times;
    
public:
    SensorDataSubscriber(const std::string& broker, int port, const std::string& topic, int qos, const std::string& encoding)
        : client(broker + ":" + std::to_string(port), "cpp_subscriber"), topic(topic), qos(qos), encoding(encoding), message_count(0) {
    }
    
    void connect() {
        mqtt::connect_options conn_opts;
        conn_opts.set_keep_alive_interval(60);
        conn_opts.set_clean_session(true);
        
        try {
            client.connect(conn_opts)->wait();
            std::cout << "✓ Connected to broker" << std::endl;
        } catch (const mqtt::exception& exc) {
            std::cerr << "Failed to connect: " << exc.what() << std::endl;
            throw;
        }
    }
    
    void subscribe() {
        try {
            client.subscribe(topic, qos)->wait();
            std::cout << "✓ Subscribed to topic: " << topic << " (QoS: " << qos << ")" << std::endl;
            std::cout << std::endl << "Waiting for messages (Ctrl+C to exit)..." << std::endl << std::endl;
        } catch (const mqtt::exception& exc) {
            std::cerr << "Failed to subscribe: " << exc.what() << std::endl;
            throw;
        }
    }
    
    void disconnect() {
        try {
            client.disconnect()->wait();
            std::cout << "✓ Disconnected from broker" << std::endl;
        } catch (const mqtt::exception& exc) {
            std::cerr << "Failed to disconnect: " << exc.what() << std::endl;
        }
    }
    
    void handle_message(const std::string& payload) {
        auto receive_time = std::chrono::system_clock::now();
        auto duration = receive_time.time_since_epoch();
        double receive_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0;
        
        message_count++;
        
        // Parse message based on encoding
        SensorData data;
        bool success = false;
        
        if (encoding == "json") {
            success = data.from_json(payload);
        } else if (encoding == "msgpack") {
            std::vector<uint8_t> msgpack_data(payload.begin(), payload.end());
            success = data.from_msgpack(msgpack_data);
        } else if (encoding == "cbor") {
            std::vector<uint8_t> cbor_data(payload.begin(), payload.end());
            success = data.from_cbor(cbor_data);
        } else if (encoding == "protobuf") {
            std::vector<uint8_t> protobuf_data(payload.begin(), payload.end());
            success = data.from_protobuf(protobuf_data);
        } else {
            std::cout << "[Message " << message_count << "] Unsupported encoding: " << encoding << std::endl;
            return;
        }
        
        if (success) {
            std::cout << "[Message " << message_count << "] Topic: " << topic << std::endl;
            std::cout << "  Sensor ID: " << data.sensor_id << std::endl;
            std::cout << "  Temperature: " << std::fixed << std::setprecision(2) << data.temperature << "°C" << std::endl;
            std::cout << "  Humidity: " << std::fixed << std::setprecision(2) << data.humidity << "%" << std::endl;
            std::cout << "  Pressure: " << std::fixed << std::setprecision(2) << data.pressure << " hPa" << std::endl;
            std::cout << "  Timestamp: " << std::fixed << std::setprecision(6) << data.timestamp << std::endl;
            
            // Calculate receive latency
            double latency = receive_timestamp - data.timestamp;
            receive_times.push_back(latency);
            std::cout << "  Receive latency: " << std::fixed << std::setprecision(2) << (latency * 1000.0) << "ms" << std::endl;
        } else {
            std::cout << "[Message " << message_count << "] Failed to decode message" << std::endl;
        }
    }
    
    void print_summary() {
        std::cout << std::endl;
        std::cout << "✓ Received " << message_count << " messages" << std::endl;
        
        if (!receive_times.empty()) {
            double total_latency = 0.0;
            for (double latency : receive_times) {
                total_latency += latency;
            }
            double avg_latency = total_latency / receive_times.size();
            std::cout << "✓ Average receive latency: " << std::fixed << std::setprecision(2) 
                     << (avg_latency * 1000.0) << "ms" << std::endl;
        }
    }
    
    void run() {
        // Set up message callback
        client.set_message_callback([this](mqtt::const_message_ptr msg) {
            handle_message(msg->to_string());
        });
        
        // Keep running until interrupted
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
};

// Global subscriber for signal handling
SensorDataSubscriber* g_subscriber = nullptr;

void signal_handler(int sig) {
    if (g_subscriber) {
        g_subscriber->print_summary();
        g_subscriber->disconnect();
        exit(0);
    }
}

int main(int argc, char* argv[]) {
    SubscriberArgs args;
    args.parse_args(argc, argv);
    
    std::cout << "=== MQTT Subscriber (C++) ===" << std::endl;
    std::cout << "Broker: " << args.broker << ":" << args.port << std::endl;
    std::cout << "Topic: " << args.topic << std::endl;
    std::cout << "QoS: " << args.qos << std::endl;
    std::cout << std::endl;
    
    // Set up signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    try {
        SensorDataSubscriber subscriber(args.broker, args.port, args.topic, args.qos, args.encoding);
        g_subscriber = &subscriber;
        
        subscriber.connect();
        subscriber.subscribe();
        subscriber.run();
        
    } catch (const std::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        return 1;
    }
    
    return 0;
}

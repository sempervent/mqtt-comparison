#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <iomanip>
#include <mqtt/async_client.h>
#include "sensor_data.h"

class PublisherArgs {
public:
    std::string broker = "localhost";
    int port = 1883;
    std::string topic = "mqtt-demo/all";
    std::string sensor_id = "sensor_001";
    int count = 10;
    double interval = 1.0;
    std::string payload_size = "small";
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
            } else if (arg == "--sensor-id" && i + 1 < argc) {
                sensor_id = argv[++i];
            } else if (arg == "--count" && i + 1 < argc) {
                count = std::stoi(argv[++i]);
            } else if (arg == "--interval" && i + 1 < argc) {
                interval = std::stod(argv[++i]);
            } else if (arg == "--payload" && i + 1 < argc) {
                payload_size = argv[++i];
            } else if (arg == "--qos" && i + 1 < argc) {
                qos = std::stoi(argv[++i]);
            } else if (arg == "--encoding" && i + 1 < argc) {
                encoding = argv[++i];
            }
        }
    }
};

class SensorDataPublisher {
private:
    mqtt::async_client client;
    std::string topic;
    int qos;
    
public:
    SensorDataPublisher(const std::string& broker, int port, const std::string& topic, int qos)
        : client(broker + ":" + std::to_string(port), "cpp_publisher"), topic(topic), qos(qos) {
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
    
    void disconnect() {
        try {
            client.disconnect()->wait();
            std::cout << "✓ Disconnected from broker" << std::endl;
        } catch (const mqtt::exception& exc) {
            std::cerr << "Failed to disconnect: " << exc.what() << std::endl;
        }
    }
    
    double publish(const SensorData& data, const std::string& encoding) {
        auto start = std::chrono::high_resolution_clock::now();
        
        std::string payload;
        if (encoding == "json") {
            payload = data.to_json();
        } else if (encoding == "msgpack") {
            auto msgpack_data = data.to_msgpack();
            payload = std::string(msgpack_data.begin(), msgpack_data.end());
        } else if (encoding == "cbor") {
            auto cbor_data = data.to_cbor();
            payload = std::string(cbor_data.begin(), cbor_data.end());
        } else if (encoding == "protobuf") {
            auto protobuf_data = data.to_protobuf();
            payload = std::string(protobuf_data.begin(), protobuf_data.end());
        } else {
            std::cerr << "Unsupported encoding: " << encoding << std::endl;
            return -1.0;
        }
        
        mqtt::message_ptr msg = mqtt::make_message(topic, payload);
        msg->set_qos(qos);
        
        try {
            client.publish(msg)->wait();
        } catch (const mqtt::exception& exc) {
            std::cerr << "Failed to publish: " << exc.what() << std::endl;
            return -1.0;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count() / 1000000.0;
    }
};

int main(int argc, char* argv[]) {
    PublisherArgs args;
    args.parse_args(argc, argv);
    
    std::cout << "=== MQTT Publisher (C++) ===" << std::endl;
    std::cout << "Broker: " << args.broker << ":" << args.port << std::endl;
    std::cout << "Topic: " << args.topic << std::endl;
    std::cout << "Payload: " << args.payload_size << std::endl;
    std::cout << "QoS: " << args.qos << std::endl;
    std::cout << std::endl;
    
    try {
        SensorDataPublisher publisher(args.broker, args.port, args.topic, args.qos);
        publisher.connect();
        
        std::vector<double> publish_times;
        publish_times.reserve(args.count);
        
        for (int i = 0; i < args.count; i++) {
            std::cout << "Publishing message " << (i + 1) << "/" << args.count << "..." << std::endl;
            
            SensorData data(args.sensor_id, args.payload_size);
            double publish_time = publisher.publish(data, args.encoding);
            
            if (publish_time >= 0) {
                publish_times.push_back(publish_time);
                std::cout << "  Publish time: " << std::fixed << std::setprecision(2) 
                         << (publish_time * 1000.0) << "ms" << std::endl;
            }
            
            if (i < args.count - 1) {
                std::this_thread::sleep_for(std::chrono::duration<double>(args.interval));
            }
        }
        
        std::cout << std::endl;
        std::cout << "✓ Published " << args.count << " messages" << std::endl;
        
        if (!publish_times.empty()) {
            double total_time = 0.0;
            for (double time : publish_times) {
                total_time += time;
            }
            double avg_time = total_time / publish_times.size();
            std::cout << "✓ Average publish time: " << std::fixed << std::setprecision(2) 
                     << (avg_time * 1000.0) << "ms" << std::endl;
        }
        
        publisher.disconnect();
        
    } catch (const std::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        return 1;
    }
    
    return 0;
}

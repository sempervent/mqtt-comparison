package com.mqttcomparison;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.dataformat.cbor.CBORFactory;
import org.msgpack.core.MessageBufferPacker;
import org.msgpack.core.MessagePack;
import org.msgpack.core.MessageUnpacker;
import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import picocli.CommandLine;
import picocli.CommandLine.Command;
import picocli.CommandLine.Option;

import java.time.Instant;

/**
 * MQTT Subscriber for Java with multiple encoding support.
 */
@Command(name = "subscriber", description = "MQTT Subscriber for Java with multiple encoding support")
public class Subscriber implements MqttCallback {
    
    @Option(names = "--broker", description = "MQTT broker hostname", defaultValue = "localhost")
    private String broker;
    
    @Option(names = "--port", description = "MQTT broker port", defaultValue = "1883")
    private int port;
    
    @Option(names = "--topic", description = "MQTT topic", defaultValue = "mqtt-demo/all")
    private String topic;
    
    @Option(names = "--qos", description = "Quality of Service level", defaultValue = "1")
    private int qos;
    
    @Option(names = "--encoding", description = "Encoding format", defaultValue = "json")
    private String encoding;
    
    private MqttClient client;
    private ObjectMapper objectMapper;
    private int messageCount = 0;
    private long[] receiveTimes;
    private int maxMessages = 1000;
    
    public static void main(String[] args) {
        int exitCode = new CommandLine(new Subscriber()).execute(args);
        System.exit(exitCode);
    }
    
    public void run() {
        System.out.println("=== MQTT Subscriber (Java) ===");
        System.out.println("Broker: " + broker + ":" + port);
        System.out.println("Topic: " + topic);
        System.out.println("QoS: " + qos);
        System.out.println("Encoding: " + encoding);
        System.out.println();
        
        try {
            // Initialize object mapper based on encoding
            initializeObjectMapper();
            
            // Allocate memory for timing data
            receiveTimes = new long[maxMessages];
            
            // Create MQTT client
            String clientId = "java_subscriber";
            MemoryPersistence persistence = new MemoryPersistence();
            client = new MqttClient("tcp://" + broker + ":" + port, clientId, persistence);
            
            // Set callbacks
            client.setCallback(this);
            
            // Connect to broker
            MqttConnectOptions connOpts = new MqttConnectOptions();
            connOpts.setCleanSession(true);
            connOpts.setKeepAliveInterval(60);
            
            client.connect(connOpts);
            System.out.println("✓ Connected to " + broker + ":" + port);
            
            // Subscribe to topic
            client.subscribe(topic, qos);
            System.out.println("✓ Subscribed to topic: " + topic + " (QoS: " + qos + ")");
            System.out.println("\nWaiting for messages (Ctrl+C to exit)...\n");
            
            // Keep running until interrupted
            while (true) {
                Thread.sleep(100);
            }
            
        } catch (Exception e) {
            System.err.println("✗ Error: " + e.getMessage());
            e.printStackTrace();
        }
    }
    
    private void initializeObjectMapper() {
        switch (encoding.toLowerCase()) {
            case "json":
                objectMapper = new ObjectMapper();
                break;
            case "msgpack":
                // MessagePack will be handled separately
                objectMapper = new ObjectMapper();
                break;
            case "cbor":
                objectMapper = new ObjectMapper(new CBORFactory());
                break;
            case "protobuf":
                // For protobuf, we'll use a different approach
                objectMapper = new ObjectMapper();
                break;
            default:
                throw new IllegalArgumentException("Unsupported encoding: " + encoding);
        }
    }
    
    @Override
    public void connectionLost(Throwable cause) {
        System.err.println("Connection lost: " + cause.getMessage());
    }
    
    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        long receiveTime = Instant.now().toEpochMilli();
        messageCount++;
        
        try {
            // Decode message
            SensorData data = decodeMessage(message.getPayload());
            
            System.out.println("\n[Message " + messageCount + "] Topic: " + topic);
            System.out.println("  Sensor ID: " + data.getSensorId());
            System.out.println("  Temperature: " + String.format("%.2f", data.getTemperature()) + "°C");
            System.out.println("  Humidity: " + String.format("%.2f", data.getHumidity()) + "%");
            System.out.println("  Pressure: " + String.format("%.2f", data.getPressure()) + " hPa");
            System.out.println("  Timestamp: " + String.format("%.6f", data.getTimestamp()));
            
            // Calculate receive latency
            long latency = receiveTime - (long) (data.getTimestamp() * 1000);
            if (messageCount <= maxMessages) {
                receiveTimes[messageCount - 1] = latency;
            }
            System.out.println("  Receive latency: " + String.format("%.2f", latency) + "ms");
            
        } catch (Exception e) {
            System.out.println("\n[Message " + messageCount + "] Failed to decode message: " + e.getMessage());
        }
    }
    
    private SensorData decodeMessage(byte[] payload) throws Exception {
        switch (encoding.toLowerCase()) {
            case "json":
            case "cbor":
                return objectMapper.readValue(payload, SensorData.class);
            case "msgpack":
                MessageUnpacker unpacker = MessagePack.newDefaultUnpacker(payload);
                SensorData data = new SensorData();
                
                // Unpack the map
                int mapSize = unpacker.unpackMapHeader();
                for (int i = 0; i < mapSize; i++) {
                    String key = unpacker.unpackString();
                    switch (key) {
                        case "timestamp":
                            data.setTimestamp(unpacker.unpackDouble());
                            break;
                        case "sensor_id":
                            data.setSensorId(unpacker.unpackString());
                            break;
                        case "temperature":
                            data.setTemperature(unpacker.unpackDouble());
                            break;
                        case "humidity":
                            data.setHumidity(unpacker.unpackDouble());
                            break;
                        case "pressure":
                            data.setPressure(unpacker.unpackDouble());
                            break;
                        default:
                            // Skip unknown fields
                            unpacker.unpackValue();
                            break;
                    }
                }
                unpacker.close();
                return data;
            case "protobuf":
                // For protobuf, we would need to convert from protobuf message
                // For now, fall back to JSON
                return objectMapper.readValue(payload, SensorData.class);
            default:
                throw new IllegalArgumentException("Unsupported encoding: " + encoding);
        }
    }
    
    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {
        // Not used in subscriber
    }
    
    public void printSummary() {
        System.out.println("\n✓ Received " + messageCount + " messages");
        
        if (messageCount > 0) {
            long totalLatency = 0;
            int validTimes = 0;
            for (int i = 0; i < Math.min(messageCount, maxMessages); i++) {
                if (receiveTimes[i] > 0) {
                    totalLatency += receiveTimes[i];
                    validTimes++;
                }
            }
            if (validTimes > 0) {
                double avgLatency = totalLatency / (double) validTimes;
                System.out.println("✓ Average receive latency: " + String.format("%.2f", avgLatency) + "ms");
            }
        }
        
        System.out.println("✓ Disconnected");
    }
}

package com.mqttcomparison;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.dataformat.cbor.CBORFactory;
import org.msgpack.core.MessageBufferPacker;
import org.msgpack.core.MessagePack;
import org.eclipse.paho.client.mqttv3.*;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import picocli.CommandLine;
import picocli.CommandLine.Command;
import picocli.CommandLine.Option;

import java.util.concurrent.TimeUnit;

/**
 * MQTT Publisher for Java with multiple encoding support.
 */
@Command(name = "publisher", description = "MQTT Publisher for Java with multiple encoding support")
public class Publisher implements MqttCallback {
    
    @Option(names = "--broker", description = "MQTT broker hostname", defaultValue = "localhost")
    private String broker;
    
    @Option(names = "--port", description = "MQTT broker port", defaultValue = "1883")
    private int port;
    
    @Option(names = "--topic", description = "MQTT topic", defaultValue = "mqtt-demo/all")
    private String topic;
    
    @Option(names = "--sensor-id", description = "Sensor identifier", defaultValue = "sensor_001")
    private String sensorId;
    
    @Option(names = "--count", description = "Number of messages to publish", defaultValue = "10")
    private int count;
    
    @Option(names = "--interval", description = "Interval between messages in seconds", defaultValue = "1.0")
    private double interval;
    
    @Option(names = "--payload", description = "Payload size variant", defaultValue = "small")
    private String payloadSize;
    
    @Option(names = "--qos", description = "Quality of Service level", defaultValue = "1")
    private int qos;
    
    @Option(names = "--encoding", description = "Encoding format", defaultValue = "json")
    private String encoding;
    
    private MqttClient client;
    private ObjectMapper objectMapper;
    private long[] publishTimes;
    
    public static void main(String[] args) {
        int exitCode = new CommandLine(new Publisher()).execute(args);
        System.exit(exitCode);
    }
    
    public void run() {
        System.out.println("=== MQTT Publisher (Java) ===");
        System.out.println("Broker: " + broker + ":" + port);
        System.out.println("Topic: " + topic);
        System.out.println("Payload: " + payloadSize);
        System.out.println("QoS: " + qos);
        System.out.println("Encoding: " + encoding);
        System.out.println();
        
        try {
            // Initialize object mapper based on encoding
            initializeObjectMapper();
            
            // Create MQTT client
            String clientId = "java_publisher";
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
            
            // Publish messages
            publishTimes = new long[count];
            
            for (int i = 0; i < count; i++) {
                System.out.println("Publishing message " + (i + 1) + "/" + count + "...");
                
                // Create sensor data
                SensorData data = new SensorData(sensorId, payloadSize);
                
                // Measure publish time
                long startTime = System.nanoTime();
                
                // Encode message
                byte[] payload = encodeMessage(data);
                
                // Publish message
                MqttMessage message = new MqttMessage(payload);
                message.setQos(qos);
                client.publish(topic, message);
                
                long endTime = System.nanoTime();
                long publishTime = endTime - startTime;
                publishTimes[i] = publishTime;
                
                System.out.println("  Publish time: " + (publishTime / 1_000_000.0) + "ms");
                
                if (i < count - 1) {
                    Thread.sleep((long) (interval * 1000));
                }
            }
            
            System.out.println();
            System.out.println("✓ Published " + count + " messages");
            
            // Calculate average publish time
            long totalTime = 0;
            for (long time : publishTimes) {
                totalTime += time;
            }
            double avgTime = totalTime / (double) count / 1_000_000.0;
            System.out.println("✓ Average publish time: " + String.format("%.2f", avgTime) + "ms");
            
            // Disconnect
            client.disconnect();
            System.out.println("✓ Disconnected");
            
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
    
    private byte[] encodeMessage(SensorData data) throws Exception {
        switch (encoding.toLowerCase()) {
            case "json":
            case "cbor":
                return objectMapper.writeValueAsBytes(data);
            case "msgpack":
                MessageBufferPacker packer = MessagePack.newDefaultBufferPacker();
                packer.packMapHeader(5); // Basic 5 fields
                packer.packString("timestamp");
                packer.packDouble(data.getTimestamp());
                packer.packString("sensor_id");
                packer.packString(data.getSensorId());
                packer.packString("temperature");
                packer.packDouble(data.getTemperature());
                packer.packString("humidity");
                packer.packDouble(data.getHumidity());
                packer.packString("pressure");
                packer.packDouble(data.getPressure());
                return packer.toByteArray();
            case "protobuf":
                // For protobuf, we would need to convert to protobuf message
                // For now, fall back to JSON
                return objectMapper.writeValueAsBytes(data);
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
        // Not used in publisher
    }
    
    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {
        // Message delivered successfully
    }
}

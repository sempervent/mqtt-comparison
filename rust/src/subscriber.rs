use clap::Parser;
use rumqttc::{Client, MqttOptions, QoS, Event, Incoming};
use serde::{Deserialize, Serialize};
use serde_json;
use rmp_serde;
use ciborium;
use prost;
use std::time::{SystemTime, UNIX_EPOCH};

#[derive(Parser)]
#[command(name = "mqtt-subscriber")]
#[command(about = "MQTT Subscriber for Rust with multiple encoding support")]
struct Args {
    #[arg(long, default_value = "localhost")]
    broker: String,
    
    #[arg(long, default_value = "1883")]
    port: u16,
    
    #[arg(long, default_value = "json")]
    encoding: String,
    
    #[arg(long, default_value = "mqtt-demo/all")]
    topic: String,
    
    #[arg(long, default_value = "1")]
    qos: u8,
}

#[derive(Serialize, Deserialize, Debug)]
struct SensorData {
    timestamp: f64,
    sensor_id: String,
    temperature: f64,
    humidity: f64,
    pressure: f64,
    location: Option<Location>,
    status: Option<String>,
    battery_level: Option<f64>,
    signal_strength: Option<i32>,
    sensor_readings: Option<Vec<f64>>,
    metadata: Option<Metadata>,
    additional_data: Option<String>,
}

#[derive(Serialize, Deserialize, Debug)]
struct Location {
    lat: f64,
    lon: f64,
    altitude: f64,
}

#[derive(Serialize, Deserialize, Debug)]
struct Metadata {
    firmware_version: String,
    hardware_id: String,
    calibration_date: String,
    last_maintenance: String,
}

struct SensorDataSubscriber {
    client: Client,
    encoding: String,
    qos: QoS,
    message_count: u32,
    receive_times: Vec<f64>,
}

impl SensorDataSubscriber {
    fn new(broker: String, port: u16, encoding: String, qos: u8) -> Self {
        let mut mqttoptions = MqttOptions::new("rust-subscriber", broker, port);
        mqttoptions.set_keep_alive(std::time::Duration::from_secs(60));
        
        let (client, _eventloop) = Client::new(mqttoptions, 10);
        
        let qos = match qos {
            0 => QoS::AtMostOnce,
            1 => QoS::AtLeastOnce,
            2 => QoS::ExactlyOnce,
            _ => QoS::AtLeastOnce,
        };
        
        Self {
            client,
            encoding,
            qos,
            message_count: 0,
            receive_times: Vec::new(),
        }
    }
    
    fn decode_message(&self, payload: &[u8]) -> Result<SensorData, Box<dyn std::error::Error>> {
        match self.encoding.as_str() {
            "json" => {
                let data: SensorData = serde_json::from_slice(payload)?;
                Ok(data)
            },
            "msgpack" => {
                let data: SensorData = rmp_serde::from_slice(payload)?;
                Ok(data)
            },
            "cbor" => {
                let data: SensorData = ciborium::de::from_reader(payload)?;
                Ok(data)
            },
            "protobuf" => {
                // For protobuf, we would need to convert from protobuf message
                // For now, fall back to JSON
                let data: SensorData = serde_json::from_slice(payload)?;
                Ok(data)
            },
            _ => Err("Unsupported encoding".into()),
        }
    }
    
    fn connect(&mut self, topic: &str) -> Result<(), Box<dyn std::error::Error>> {
        println!("Connecting to MQTT broker at {}:{}...", 
                 self.client.options().broker_address(), 
                 self.client.options().port());
        
        self.client.subscribe(topic, self.qos)?;
        println!("✓ Subscribed to topic: {} (QoS: {:?})", topic, self.qos);
        println!("\nWaiting for messages (Ctrl+C to exit)...\n");
        
        Ok(())
    }
    
    fn handle_message(&mut self, topic: &str, payload: &[u8]) {
        let receive_time = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_secs_f64();
        
        match self.decode_message(payload) {
            Ok(data) => {
                self.message_count += 1;
                
                // Calculate receive latency if timestamp is available
                let latency = receive_time - data.timestamp;
                self.receive_times.push(latency);
                
                println!("\n[Message {}] Topic: {}", self.message_count, topic);
                println!("  Sensor ID: {}", data.sensor_id);
                println!("  Temperature: {}°C", data.temperature);
                println!("  Humidity: {}%", data.humidity);
                println!("  Pressure: {} hPa", data.pressure);
                println!("  Timestamp: {}", data.timestamp);
                println!("  Receive latency: {:.2}ms", latency * 1000.0);
            },
            Err(e) => {
                eprintln!("Error decoding message: {}", e);
            }
        }
    }
    
    fn disconnect(&self) {
        println!("\n✓ Received {} messages", self.message_count);
        if !self.receive_times.is_empty() {
            let avg_latency = self.receive_times.iter().sum::<f64>() / self.receive_times.len() as f64;
            println!("✓ Average receive latency: {:.2}ms", avg_latency * 1000.0);
        }
        println!("✓ Disconnected");
    }
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::parse();
    
    println!("=== MQTT Subscriber (Rust) ===");
    println!("Encoding: {}", args.encoding);
    println!("Topic: {}", args.topic);
    println!("QoS: {}", args.qos);
    println!();
    
    let mut subscriber = SensorDataSubscriber::new(
        args.broker,
        args.port,
        args.encoding,
        args.qos,
    );
    
    subscriber.connect(&args.topic)?;
    
    // Create a new client for the event loop
    let mut mqttoptions = MqttOptions::new("rust-subscriber-loop", args.broker, args.port);
    mqttoptions.set_keep_alive(std::time::Duration::from_secs(60));
    
    let (mut client, mut eventloop) = Client::new(mqttoptions, 10);
    client.subscribe(&args.topic, subscriber.qos)?;
    
    loop {
        match eventloop.poll().await {
            Ok(Event::Incoming(Incoming::Publish(publish))) => {
                subscriber.handle_message(&publish.topic, &publish.payload);
            },
            Ok(_) => {},
            Err(e) => {
                eprintln!("Error in event loop: {}", e);
                break;
            }
        }
    }
    
    subscriber.disconnect();
    Ok(())
}

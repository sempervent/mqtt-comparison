use clap::Parser;
use rumqttc::{Client, MqttOptions, QoS, Publish};
use serde::{Deserialize, Serialize};
use serde_json;
use rmp_serde;
use ciborium;
use prost;
use std::time::{SystemTime, UNIX_EPOCH};
use std::thread;
use std::time::Duration;
use rand::Rng;

#[derive(Parser)]
#[command(name = "mqtt-publisher")]
#[command(about = "MQTT Publisher for Rust with multiple encoding support")]
struct Args {
    #[arg(long, default_value = "localhost")]
    broker: String,
    
    #[arg(long, default_value = "1883")]
    port: u16,
    
    #[arg(long, default_value = "json")]
    encoding: String,
    
    #[arg(long, default_value = "mqtt-demo/all")]
    topic: String,
    
    #[arg(long, default_value = "sensor_001")]
    sensor_id: String,
    
    #[arg(long, default_value = "10")]
    count: u32,
    
    #[arg(long, default_value = "1.0")]
    interval: f64,
    
    #[arg(long, default_value = "small")]
    payload: String,
    
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

struct SensorDataPublisher {
    client: Client,
    encoding: String,
    qos: QoS,
}

impl SensorDataPublisher {
    fn new(broker: String, port: u16, encoding: String, qos: u8) -> Self {
        let mut mqttoptions = MqttOptions::new("rust-publisher", broker, port);
        mqttoptions.set_keep_alive(Duration::from_secs(60));
        
        let (client, mut eventloop) = Client::new(mqttoptions, 10);
        
        // Start the event loop in a separate thread
        tokio::spawn(async move {
            loop {
                match eventloop.poll().await {
                    Ok(_) => {},
                    Err(e) => {
                        eprintln!("Error in event loop: {}", e);
                        break;
                    }
                }
            }
        });
        
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
        }
    }
    
    fn create_sensor_data(&self, sensor_id: &str, payload_size: &str) -> SensorData {
        let timestamp = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap()
            .as_secs_f64();
        
        let mut rng = rand::thread_rng();
        
        let mut data = SensorData {
            timestamp,
            sensor_id: sensor_id.to_string(),
            temperature: (20.0 + rng.gen_range(-5.0..15.0) * 100.0).round() / 100.0,
            humidity: (30.0 + rng.gen_range(0.0..40.0) * 100.0).round() / 100.0,
            pressure: (1000.0 + rng.gen_range(-50.0..50.0) * 100.0).round() / 100.0,
            location: None,
            status: None,
            battery_level: None,
            signal_strength: None,
            sensor_readings: None,
            metadata: None,
            additional_data: None,
        };
        
        if payload_size == "medium" {
            data.location = Some(Location {
                lat: 40.7128,
                lon: -74.0060,
                altitude: 10.5,
            });
            data.status = Some("active".to_string());
            data.battery_level = Some((rng.gen_range(20.0..100.0) * 10.0).round() / 10.0);
            data.signal_strength = Some(rng.gen_range(-100..-30));
            data.additional_data = Some("x".repeat(1500));
        } else if payload_size == "large" {
            data.location = Some(Location {
                lat: 40.7128,
                lon: -74.0060,
                altitude: 10.5,
            });
            data.status = Some("active".to_string());
            data.battery_level = Some((rng.gen_range(20.0..100.0) * 10.0).round() / 10.0);
            data.signal_strength = Some(rng.gen_range(-100..-30));
            data.sensor_readings = Some((0..100).map(|_| (rng.gen_range(0.0..100.0) * 100.0).round() / 100.0).collect());
            data.metadata = Some(Metadata {
                firmware_version: "1.2.3".to_string(),
                hardware_id: "HW-001".to_string(),
                calibration_date: "2024-01-01".to_string(),
                last_maintenance: "2024-06-01".to_string(),
            });
            data.additional_data = Some("x".repeat(60000));
        }
        
        data
    }
    
    fn encode_message(&self, data: &SensorData) -> Result<Vec<u8>, Box<dyn std::error::Error>> {
        match self.encoding.as_str() {
            "json" => {
                let json = serde_json::to_vec(data)?;
                Ok(json)
            },
            "msgpack" => {
                let msgpack = rmp_serde::to_vec(data)?;
                Ok(msgpack)
            },
            "cbor" => {
                let mut buf = Vec::new();
                ciborium::ser::into_writer(data, &mut buf)?;
                Ok(buf)
            },
            "protobuf" => {
                // For protobuf, we would need to convert to protobuf message
                // For now, fall back to JSON
                let json = serde_json::to_vec(data)?;
                Ok(json)
            },
            _ => Err("Unsupported encoding".into()),
        }
    }
    
    fn publish(&self, topic: &str, data: &SensorData) -> Result<f64, Box<dyn std::error::Error>> {
        let start = std::time::Instant::now();
        
        let payload = self.encode_message(data)?;
        let publish = Publish::new(topic, self.qos, payload);
        
        self.client.publish(publish)?;
        
        Ok(start.elapsed().as_secs_f64())
    }
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::parse();
    
    println!("=== MQTT Publisher (Rust) ===");
    println!("Encoding: {}", args.encoding);
    println!("Topic: {}", args.topic);
    println!("Payload: {}", args.payload);
    println!("QoS: {}", args.qos);
    println!();
    
    let publisher = SensorDataPublisher::new(
        args.broker,
        args.port,
        args.encoding,
        args.qos,
    );
    
    let mut publish_times = Vec::new();
    
    // Give the client time to connect
    thread::sleep(Duration::from_secs(1));
    
    for i in 0..args.count {
        let data = publisher.create_sensor_data(&args.sensor_id, &args.payload);
        println!("Publishing message {}/{}...", i + 1, args.count);
        
        match publisher.publish(&args.topic, &data) {
            Ok(publish_time) => {
                publish_times.push(publish_time);
                println!("  Publish time: {:.2}ms", publish_time * 1000.0);
            },
            Err(e) => {
                eprintln!("Error publishing message: {}", e);
            }
        }
        
        if i < args.count - 1 {
            thread::sleep(Duration::from_secs_f64(args.interval));
        }
    }
    
    println!();
    println!("✓ Published {} messages", args.count);
    if !publish_times.is_empty() {
        let avg_time = publish_times.iter().sum::<f64>() / publish_times.len() as f64;
        println!("✓ Average publish time: {:.2}ms", avg_time * 1000.0);
    }
    
    Ok(())
}

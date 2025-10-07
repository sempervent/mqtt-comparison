#!/usr/bin/env python3
"""
MQTT Publisher for Python with multiple encoding support.

Publishes sensor data to MQTT broker using JSON or MessagePack encoding.
"""

import json
import time
import argparse
import random
import os
from typing import Dict, Any
import paho.mqtt.client as mqtt

try:
    import msgpack
    MSGPACK_AVAILABLE = True
except ImportError:
    MSGPACK_AVAILABLE = False

try:
    import cbor2
    CBOR_AVAILABLE = True
except ImportError:
    CBOR_AVAILABLE = False

try:
    import sensor_data_pb2
    PROTOBUF_AVAILABLE = True
except (ImportError, TypeError):
    PROTOBUF_AVAILABLE = False


class SensorDataPublisher:
    """Publisher for sensor data messages."""

    def __init__(self, broker: str = "localhost", port: int = 1883, encoding: str = "json", qos: int = 1):
        """
        Initialize the publisher.

        Args:
            broker: MQTT broker hostname
            port: MQTT broker port
            encoding: Encoding format ('json' or 'msgpack')
            qos: Quality of Service level (0, 1, or 2)
        """
        self.broker = broker
        self.port = port
        self.encoding = encoding.lower()
        self.qos = qos
        self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        self.client.on_connect = self._on_connect
        self.client.on_publish = self._on_publish

    def _on_connect(self, client, userdata, flags, reason_code, properties):
        """Callback for when the client connects to the broker."""
        if reason_code == 0:
            print(f"✓ Connected to {self.broker}:{self.port}")
        else:
            print(f"✗ Connection failed with code: {reason_code}")

    def _on_publish(self, client, userdata, mid, reason_code, properties):
        """Callback for when a message is published."""
        print(f"  Published message {mid}")

    def connect(self):
        """Connect to the MQTT broker."""
        print(f"Connecting to MQTT broker at {self.broker}:{self.port}...")
        self.client.connect(self.broker, self.port, 60)
        self.client.loop_start()
        time.sleep(1)  # Wait for connection

    def disconnect(self):
        """Disconnect from the MQTT broker."""
        self.client.loop_stop()
        self.client.disconnect()

    def encode_message(self, data: Dict[str, Any]) -> bytes:
        """
        Encode message data based on configured encoding.

        Args:
            data: Dictionary containing sensor data

        Returns:
            Encoded message as bytes
        """
        if self.encoding == "json":
            return json.dumps(data).encode('utf-8')
        elif self.encoding == "msgpack":
            if not MSGPACK_AVAILABLE:
                raise ImportError("msgpack is not installed")
            return msgpack.packb(data)
        elif self.encoding == "cbor":
            if not CBOR_AVAILABLE:
                raise ImportError("cbor2 is not installed")
            return cbor2.dumps(data)
        elif self.encoding == "protobuf":
            if not PROTOBUF_AVAILABLE:
                raise ImportError("protobuf is not installed")
            # Convert dict to protobuf message
            pb_message = sensor_data_pb2.SensorData()
            pb_message.timestamp = data.get('timestamp', 0.0)
            pb_message.sensor_id = data.get('sensor_id', '')
            pb_message.temperature = data.get('temperature', 0.0)
            pb_message.humidity = data.get('humidity', 0.0)
            pb_message.pressure = data.get('pressure', 0.0)
            return pb_message.SerializeToString()
        else:
            raise ValueError(f"Unsupported encoding: {self.encoding}")

    def create_sensor_data(self, sensor_id: str, payload_size: str = "small") -> Dict[str, Any]:
        """
        Create sample sensor data with configurable payload size.

        Args:
            sensor_id: Sensor identifier
            payload_size: Size variant ('small', 'medium', 'large')

        Returns:
            Dictionary with sensor data
        """
        base_data = {
            "timestamp": time.time(),
            "sensor_id": sensor_id,
            "temperature": round(20 + random.uniform(-5, 15), 2),
            "humidity": round(30 + random.uniform(0, 40), 2),
            "pressure": round(1000 + random.uniform(-50, 50), 2)
        }
        
        if payload_size == "small":
            return base_data
        elif payload_size == "medium":
            # Add more fields to reach ~2KB
            base_data.update({
                "location": {"lat": 40.7128, "lon": -74.0060, "altitude": 10.5},
                "status": "active",
                "battery_level": round(random.uniform(20, 100), 1),
                "signal_strength": random.randint(-100, -30),
                "additional_data": "x" * 1500  # Pad to reach ~2KB
            })
        elif payload_size == "large":
            # Add even more fields to reach ~64KB
            base_data.update({
                "location": {"lat": 40.7128, "lon": -74.0060, "altitude": 10.5},
                "status": "active",
                "battery_level": round(random.uniform(20, 100), 1),
                "signal_strength": random.randint(-100, -30),
                "sensor_readings": [round(random.uniform(0, 100), 2) for _ in range(100)],
                "metadata": {
                    "firmware_version": "1.2.3",
                    "hardware_id": "HW-001",
                    "calibration_date": "2024-01-01",
                    "last_maintenance": "2024-06-01"
                },
                "additional_data": "x" * 60000  # Pad to reach ~64KB
            })
        
        return base_data

    def publish(self, topic: str, data: Dict[str, Any]) -> float:
        """
        Publish sensor data to MQTT topic.

        Args:
            topic: MQTT topic
            data: Sensor data dictionary

        Returns:
            Time taken to publish in seconds
        """
        start_time = time.time()
        payload = self.encode_message(data)
        result = self.client.publish(topic, payload, qos=self.qos)
        result.wait_for_publish()
        return time.time() - start_time


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(description="MQTT Sensor Data Publisher")
    parser.add_argument("--broker", default=os.getenv("MQTT_BROKER", "localhost"), help="MQTT broker hostname")
    parser.add_argument("--port", type=int, default=int(os.getenv("MQTT_PORT", "1883")), help="MQTT broker port")
    parser.add_argument("--encoding", choices=["json", "msgpack", "cbor", "protobuf"], default="json",
                        help="Encoding format")
    parser.add_argument("--topic", default="mqtt-demo/all", help="MQTT topic")
    parser.add_argument("--sensor-id", default="sensor_001", help="Sensor ID")
    parser.add_argument("--count", type=int, default=10, help="Number of messages to publish")
    parser.add_argument("--interval", type=float, default=1.0, help="Interval between messages (seconds)")
    parser.add_argument("--payload", choices=["small", "medium", "large"], default="small",
                        help="Payload size variant")
    parser.add_argument("--qos", type=int, choices=[0, 1, 2], default=1,
                        help="Quality of Service level")

    args = parser.parse_args()

    print(f"=== MQTT Publisher (Python) ===")
    print(f"Encoding: {args.encoding}")
    print(f"Topic: {args.topic}")
    print(f"Payload: {args.payload}")
    print(f"QoS: {args.qos}")
    print()

    publisher = SensorDataPublisher(args.broker, args.port, args.encoding, args.qos)
    publish_times = []

    try:
        publisher.connect()

        for i in range(args.count):
            data = publisher.create_sensor_data(args.sensor_id, args.payload)
            print(f"Publishing message {i+1}/{args.count}...")
            publish_time = publisher.publish(args.topic, data)
            publish_times.append(publish_time)
            print(f"  Publish time: {publish_time*1000:.2f}ms")
            if i < args.count - 1:
                time.sleep(args.interval)

        print()
        print(f"✓ Published {args.count} messages")
        if publish_times:
            avg_time = sum(publish_times) / len(publish_times)
            print(f"✓ Average publish time: {avg_time*1000:.2f}ms")

    except KeyboardInterrupt:
        print("\n✗ Interrupted by user")
    except Exception as e:
        print(f"\n✗ Error: {e}")
    finally:
        publisher.disconnect()


if __name__ == "__main__":
    main()

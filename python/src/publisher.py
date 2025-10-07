#!/usr/bin/env python3
"""
MQTT Publisher for Python with multiple encoding support.

Publishes sensor data to MQTT broker using JSON or MessagePack encoding.
"""

import json
import time
import argparse
from typing import Dict, Any
import paho.mqtt.client as mqtt

try:
    import msgpack
    MSGPACK_AVAILABLE = True
except ImportError:
    MSGPACK_AVAILABLE = False


class SensorDataPublisher:
    """Publisher for sensor data messages."""

    def __init__(self, broker: str = "localhost", port: int = 1883, encoding: str = "json"):
        """
        Initialize the publisher.

        Args:
            broker: MQTT broker hostname
            port: MQTT broker port
            encoding: Encoding format ('json' or 'msgpack')
        """
        self.broker = broker
        self.port = port
        self.encoding = encoding.lower()
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
        else:
            raise ValueError(f"Unsupported encoding: {self.encoding}")

    def create_sensor_data(self, sensor_id: str) -> Dict[str, Any]:
        """
        Create sample sensor data.

        Args:
            sensor_id: Sensor identifier

        Returns:
            Dictionary with sensor data
        """
        return {
            "timestamp": time.time(),
            "sensor_id": sensor_id,
            "temperature": 23.5,
            "humidity": 45.2,
            "pressure": 1013.25
        }

    def publish(self, topic: str, data: Dict[str, Any]):
        """
        Publish sensor data to MQTT topic.

        Args:
            topic: MQTT topic
            data: Sensor data dictionary
        """
        payload = self.encode_message(data)
        result = self.client.publish(topic, payload)
        result.wait_for_publish()


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(description="MQTT Sensor Data Publisher")
    parser.add_argument("--broker", default="localhost", help="MQTT broker hostname")
    parser.add_argument("--port", type=int, default=1883, help="MQTT broker port")
    parser.add_argument("--encoding", choices=["json", "msgpack"], default="json",
                        help="Encoding format")
    parser.add_argument("--topic", default="sensors/data", help="MQTT topic")
    parser.add_argument("--sensor-id", default="sensor_001", help="Sensor ID")
    parser.add_argument("--count", type=int, default=10, help="Number of messages to publish")
    parser.add_argument("--interval", type=float, default=1.0, help="Interval between messages (seconds)")

    args = parser.parse_args()

    print(f"=== MQTT Publisher (Python) ===")
    print(f"Encoding: {args.encoding}")
    print(f"Topic: {args.topic}")
    print()

    publisher = SensorDataPublisher(args.broker, args.port, args.encoding)

    try:
        publisher.connect()

        for i in range(args.count):
            data = publisher.create_sensor_data(args.sensor_id)
            print(f"Publishing message {i+1}/{args.count}...")
            publisher.publish(args.topic, data)
            if i < args.count - 1:
                time.sleep(args.interval)

        print()
        print(f"✓ Published {args.count} messages")

    except KeyboardInterrupt:
        print("\n✗ Interrupted by user")
    except Exception as e:
        print(f"\n✗ Error: {e}")
    finally:
        publisher.disconnect()


if __name__ == "__main__":
    main()

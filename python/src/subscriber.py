#!/usr/bin/env python3
"""
MQTT Subscriber for Python with multiple encoding support.

Subscribes to sensor data from MQTT broker using JSON or MessagePack encoding.
"""

import json
import argparse
from typing import Any
import paho.mqtt.client as mqtt

try:
    import msgpack
    MSGPACK_AVAILABLE = True
except ImportError:
    MSGPACK_AVAILABLE = False


class SensorDataSubscriber:
    """Subscriber for sensor data messages."""

    def __init__(self, broker: str = "localhost", port: int = 1883, encoding: str = "json"):
        """
        Initialize the subscriber.

        Args:
            broker: MQTT broker hostname
            port: MQTT broker port
            encoding: Encoding format ('json' or 'msgpack')
        """
        self.broker = broker
        self.port = port
        self.encoding = encoding.lower()
        self.message_count = 0
        self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        self.client.on_connect = self._on_connect
        self.client.on_message = self._on_message

    def _on_connect(self, client, userdata, flags, reason_code, properties):
        """Callback for when the client connects to the broker."""
        if reason_code == 0:
            print(f"✓ Connected to {self.broker}:{self.port}")
        else:
            print(f"✗ Connection failed with code: {reason_code}")

    def _on_message(self, client, userdata, msg):
        """Callback for when a message is received."""
        try:
            data = self.decode_message(msg.payload)
            self.message_count += 1
            print(f"\n[Message {self.message_count}] Topic: {msg.topic}")
            print(f"  Sensor ID: {data.get('sensor_id', 'N/A')}")
            print(f"  Temperature: {data.get('temperature', 'N/A')}°C")
            print(f"  Humidity: {data.get('humidity', 'N/A')}%")
            print(f"  Pressure: {data.get('pressure', 'N/A')} hPa")
            print(f"  Timestamp: {data.get('timestamp', 'N/A')}")
        except Exception as e:
            print(f"✗ Error decoding message: {e}")

    def decode_message(self, payload: bytes) -> Any:
        """
        Decode message payload based on configured encoding.

        Args:
            payload: Raw message payload

        Returns:
            Decoded message data
        """
        if self.encoding == "json":
            return json.loads(payload.decode('utf-8'))
        elif self.encoding == "msgpack":
            if not MSGPACK_AVAILABLE:
                raise ImportError("msgpack is not installed")
            return msgpack.unpackb(payload, raw=False)
        else:
            raise ValueError(f"Unsupported encoding: {self.encoding}")

    def connect(self, topic: str):
        """
        Connect to the MQTT broker and subscribe to topic.

        Args:
            topic: MQTT topic to subscribe to
        """
        print(f"Connecting to MQTT broker at {self.broker}:{self.port}...")
        self.client.connect(self.broker, self.port, 60)
        self.client.subscribe(topic)
        print(f"✓ Subscribed to topic: {topic}")
        print("\nWaiting for messages (Ctrl+C to exit)...\n")

    def loop(self):
        """Start the message loop."""
        self.client.loop_forever()

    def disconnect(self):
        """Disconnect from the MQTT broker."""
        self.client.disconnect()


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(description="MQTT Sensor Data Subscriber")
    parser.add_argument("--broker", default="localhost", help="MQTT broker hostname")
    parser.add_argument("--port", type=int, default=1883, help="MQTT broker port")
    parser.add_argument("--encoding", choices=["json", "msgpack"], default="json",
                        help="Encoding format")
    parser.add_argument("--topic", default="sensors/data", help="MQTT topic")

    args = parser.parse_args()

    print(f"=== MQTT Subscriber (Python) ===")
    print(f"Encoding: {args.encoding}")
    print(f"Topic: {args.topic}")
    print()

    subscriber = SensorDataSubscriber(args.broker, args.port, args.encoding)

    try:
        subscriber.connect(args.topic)
        subscriber.loop()
    except KeyboardInterrupt:
        print(f"\n\n✓ Received {subscriber.message_count} messages")
        print("✓ Disconnected")
    except Exception as e:
        print(f"\n✗ Error: {e}")
    finally:
        subscriber.disconnect()


if __name__ == "__main__":
    main()

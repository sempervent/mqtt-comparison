#!/usr/bin/env python3
"""
MQTT Subscriber for Python with multiple encoding support.

Subscribes to sensor data from MQTT broker using JSON or MessagePack encoding.
"""

import json
import argparse
import time
import os
from typing import Any
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


class SensorDataSubscriber:
    """Subscriber for sensor data messages."""

    def __init__(self, broker: str = "localhost", port: int = 1883, encoding: str = "json", qos: int = 1):
        """
        Initialize the subscriber.

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
        self.message_count = 0
        self.receive_times = []
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
            receive_time = time.time()
            data = self.decode_message(msg.payload)
            self.message_count += 1
            
            # Calculate receive latency if timestamp is available
            if 'timestamp' in data:
                latency = receive_time - data['timestamp']
                self.receive_times.append(latency)
            
            print(f"\n[Message {self.message_count}] Topic: {msg.topic}")
            print(f"  Sensor ID: {data.get('sensor_id', 'N/A')}")
            print(f"  Temperature: {data.get('temperature', 'N/A')}°C")
            print(f"  Humidity: {data.get('humidity', 'N/A')}%")
            print(f"  Pressure: {data.get('pressure', 'N/A')} hPa")
            print(f"  Timestamp: {data.get('timestamp', 'N/A')}")
            if 'timestamp' in data:
                print(f"  Receive latency: {latency*1000:.2f}ms")
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
        elif self.encoding == "cbor":
            if not CBOR_AVAILABLE:
                raise ImportError("cbor2 is not installed")
            return cbor2.loads(payload)
        elif self.encoding == "protobuf":
            if not PROTOBUF_AVAILABLE:
                raise ImportError("protobuf is not installed")
            # Parse protobuf message
            pb_message = sensor_data_pb2.SensorData()
            pb_message.ParseFromString(payload)
            # Convert to dict
            return {
                'timestamp': pb_message.timestamp,
                'sensor_id': pb_message.sensor_id,
                'temperature': pb_message.temperature,
                'humidity': pb_message.humidity,
                'pressure': pb_message.pressure
            }
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
        self.client.subscribe(topic, qos=self.qos)
        print(f"✓ Subscribed to topic: {topic} (QoS: {self.qos})")
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
    parser.add_argument("--broker", default=os.getenv("MQTT_BROKER", "localhost"), help="MQTT broker hostname")
    parser.add_argument("--port", type=int, default=int(os.getenv("MQTT_PORT", "1883")), help="MQTT broker port")
    parser.add_argument("--encoding", choices=["json", "msgpack", "cbor", "protobuf"], default="json",
                        help="Encoding format")
    parser.add_argument("--topic", default="mqtt-demo/all", help="MQTT topic")
    parser.add_argument("--qos", type=int, choices=[0, 1, 2], default=1,
                        help="Quality of Service level")

    args = parser.parse_args()

    print(f"=== MQTT Subscriber (Python) ===")
    print(f"Encoding: {args.encoding}")
    print(f"Topic: {args.topic}")
    print(f"QoS: {args.qos}")
    print()

    subscriber = SensorDataSubscriber(args.broker, args.port, args.encoding, args.qos)

    try:
        subscriber.connect(args.topic)
        subscriber.loop()
    except KeyboardInterrupt:
        print(f"\n\n✓ Received {subscriber.message_count} messages")
        if subscriber.receive_times:
            avg_latency = sum(subscriber.receive_times) / len(subscriber.receive_times)
            print(f"✓ Average receive latency: {avg_latency*1000:.2f}ms")
        print("✓ Disconnected")
    except Exception as e:
        print(f"\n✗ Error: {e}")
    finally:
        subscriber.disconnect()


if __name__ == "__main__":
    main()

using MQTTnet;
using MQTTnet.Client;
using System.Diagnostics;
using MessagePack;
using PeterO.Cbor;
using Google.Protobuf;

namespace MQTTComparison
{
    public class Subscriber
    {
        private readonly IMqttClient _client;
        private readonly string _broker;
        private readonly int _port;
        private readonly string _topic;
        private readonly int _qos;
        private readonly string _encoding;
        private int _messageCount;
        private readonly List<double> _receiveTimes;

        public Subscriber(string broker, int port, string topic, int qos, string encoding = "json")
        {
            _broker = broker;
            _port = port;
            _topic = topic;
            _qos = qos;
            _encoding = encoding;
            _messageCount = 0;
            _receiveTimes = new List<double>();
            
            var factory = new MqttFactory();
            _client = factory.CreateMqttClient();
        }

        public async Task ConnectAsync()
        {
            var options = new MqttClientOptionsBuilder()
                .WithTcpServer(_broker, _port)
                .WithClientId("csharp_subscriber")
                .WithKeepAlivePeriod(TimeSpan.FromSeconds(60))
                .WithCleanSession(true)
                .Build();

            await _client.ConnectAsync(options);
            Console.WriteLine($"✓ Connected to {_broker}:{_port}");
        }

        public async Task SubscribeAsync()
        {
            _client.UseApplicationMessageReceivedHandler(OnMessageReceived);
            
            await _client.SubscribeAsync(new MqttTopicFilterBuilder()
                .WithTopic(_topic)
                .WithQualityOfServiceLevel((MQTTnet.Protocol.MqttQualityOfServiceLevel)_qos)
                .Build());
                
            Console.WriteLine($"✓ Subscribed to topic: {_topic} (QoS: {_qos})");
            Console.WriteLine("\nWaiting for messages (Ctrl+C to exit)...\n");
        }

        public async Task DisconnectAsync()
        {
            await _client.DisconnectAsync();
            Console.WriteLine("✓ Disconnected");
        }

        private SensorData DecodeMessage(byte[] payload)
        {
            return _encoding.ToLower() switch
            {
                "json" => JsonSerializer.Deserialize<SensorData>(System.Text.Encoding.UTF8.GetString(payload))!,
                "msgpack" => MessagePackSerializer.Deserialize<SensorData>(payload),
                "cbor" => CBORObject.DecodeFromBytes(payload).ToObject<SensorData>(),
                "protobuf" => JsonSerializer.Deserialize<SensorData>(System.Text.Encoding.UTF8.GetString(payload))!, // Fallback to JSON for now
                _ => throw new ArgumentException($"Unsupported encoding: {_encoding}")
            };
        }

        private async Task OnMessageReceived(MqttApplicationMessageReceivedEventArgs e)
        {
            var receiveTime = DateTimeOffset.UtcNow.ToUnixTimeSeconds();
            _messageCount++;

            try
            {
                var data = DecodeMessage(e.ApplicationMessage.Payload);

                Console.WriteLine($"\n[Message {_messageCount}] Topic: {e.ApplicationMessage.Topic}");
                Console.WriteLine($"  Sensor ID: {data.SensorId}");
                Console.WriteLine($"  Temperature: {data.Temperature:F2}°C");
                Console.WriteLine($"  Humidity: {data.Humidity:F2}%");
                Console.WriteLine($"  Pressure: {data.Pressure:F2} hPa");
                Console.WriteLine($"  Timestamp: {data.Timestamp:F6}");

                // Calculate receive latency
                var latency = receiveTime - data.Timestamp;
                _receiveTimes.Add(latency);
                Console.WriteLine($"  Receive latency: {latency * 1000:F2}ms");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"\n[Message {_messageCount}] Failed to decode message: {ex.Message}");
            }

            await Task.CompletedTask;
        }

        public void PrintSummary()
        {
            Console.WriteLine($"\n✓ Received {_messageCount} messages");
            
            if (_receiveTimes.Count > 0)
            {
                var avgLatency = _receiveTimes.Average();
                Console.WriteLine($"✓ Average receive latency: {avgLatency * 1000:F2}ms");
            }
        }
    }

    public class SubscriberProgram
    {
        private static Subscriber? _subscriber;

        public static async Task Main(string[] args)
        {
            // Parse command line arguments
            var options = ParseArgs(args);
            
            Console.WriteLine("=== MQTT Subscriber (C#) ===");
            Console.WriteLine($"Broker: {options.Broker}:{options.Port}");
            Console.WriteLine($"Topic: {options.Topic}");
            Console.WriteLine($"QoS: {options.Qos}");
            Console.WriteLine($"Encoding: {options.Encoding}");
            Console.WriteLine();

            // Set up signal handler
            Console.CancelKeyPress += (sender, e) =>
            {
                e.Cancel = true;
                _subscriber?.PrintSummary();
                _subscriber?.DisconnectAsync().Wait();
                Environment.Exit(0);
            };

            try
            {
                _subscriber = new Subscriber(options.Broker, options.Port, options.Topic, options.Qos, options.Encoding);
                await _subscriber.ConnectAsync();
                await _subscriber.SubscribeAsync();

                // Keep running until interrupted
                while (true)
                {
                    await Task.Delay(100);
                }
            }
            catch (Exception e)
            {
                Console.WriteLine($"✗ Error: {e.Message}");
                Environment.Exit(1);
            }
        }

        private static SubscriberOptions ParseArgs(string[] args)
        {
            var options = new SubscriberOptions();
            
            for (int i = 0; i < args.Length; i++)
            {
                switch (args[i])
                {
                    case "--broker" when i + 1 < args.Length:
                        options.Broker = args[++i];
                        break;
                    case "--port" when i + 1 < args.Length:
                        options.Port = int.Parse(args[++i]);
                        break;
                    case "--topic" when i + 1 < args.Length:
                        options.Topic = args[++i];
                        break;
                    case "--qos" when i + 1 < args.Length:
                        options.Qos = int.Parse(args[++i]);
                        break;
                    case "--encoding" when i + 1 < args.Length:
                        options.Encoding = args[++i];
                        break;
                }
            }
            
            return options;
        }
    }

    public class SubscriberOptions
    {
        public string Broker { get; set; } = "localhost";
        public int Port { get; set; } = 1883;
        public string Topic { get; set; } = "mqtt-demo/all";
        public int Qos { get; set; } = 1;
        public string Encoding { get; set; } = "json";
    }
}

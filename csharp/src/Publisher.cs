using MQTTnet;
using MQTTnet.Client;
using System.Diagnostics;
using MessagePack;
using PeterO.Cbor;
using Google.Protobuf;

namespace MQTTComparison
{
    public class Publisher
    {
        private readonly IMqttClient _client;
        private readonly string _broker;
        private readonly int _port;
        private readonly string _topic;
        private readonly int _qos;
        private readonly string _encoding;

        public Publisher(string broker, int port, string topic, int qos, string encoding = "json")
        {
            _broker = broker;
            _port = port;
            _topic = topic;
            _qos = qos;
            _encoding = encoding;
            
            var factory = new MqttFactory();
            _client = factory.CreateMqttClient();
        }

        public async Task ConnectAsync()
        {
            var options = new MqttClientOptionsBuilder()
                .WithTcpServer(_broker, _port)
                .WithClientId("csharp_publisher")
                .WithKeepAlivePeriod(TimeSpan.FromSeconds(60))
                .WithCleanSession(true)
                .Build();

            await _client.ConnectAsync(options);
            Console.WriteLine($"✓ Connected to {_broker}:{_port}");
        }

        public async Task DisconnectAsync()
        {
            await _client.DisconnectAsync();
            Console.WriteLine("✓ Disconnected");
        }

        private byte[] EncodeMessage(SensorData data)
        {
            return _encoding.ToLower() switch
            {
                "json" => System.Text.Encoding.UTF8.GetBytes(JsonSerializer.Serialize(data)),
                "msgpack" => MessagePackSerializer.Serialize(data),
                "cbor" => CBORObject.FromObject(data).EncodeToBytes(),
                "protobuf" => System.Text.Encoding.UTF8.GetBytes(JsonSerializer.Serialize(data)), // Fallback to JSON for now
                _ => throw new ArgumentException($"Unsupported encoding: {_encoding}")
            };
        }

        public async Task<double> PublishAsync(SensorData data)
        {
            var stopwatch = Stopwatch.StartNew();
            
            var payload = EncodeMessage(data);
            var message = new MqttApplicationMessageBuilder()
                .WithTopic(_topic)
                .WithPayload(payload)
                .WithQualityOfServiceLevel((MQTTnet.Protocol.MqttQualityOfServiceLevel)_qos)
                .Build();

            await _client.PublishAsync(message);
            
            stopwatch.Stop();
            return stopwatch.Elapsed.TotalSeconds;
        }
    }

    public class PublisherProgram
    {
        public static async Task Main(string[] args)
        {
            // Parse command line arguments
            var options = ParseArgs(args);
            
            Console.WriteLine("=== MQTT Publisher (C#) ===");
            Console.WriteLine($"Broker: {options.Broker}:{options.Port}");
            Console.WriteLine($"Topic: {options.Topic}");
            Console.WriteLine($"Payload: {options.PayloadSize}");
            Console.WriteLine($"QoS: {options.Qos}");
            Console.WriteLine($"Encoding: {options.Encoding}");
            Console.WriteLine();

            try
            {
                var publisher = new Publisher(options.Broker, options.Port, options.Topic, options.Qos, options.Encoding);
                await publisher.ConnectAsync();

                var publishTimes = new List<double>();

                for (int i = 0; i < options.Count; i++)
                {
                    Console.WriteLine($"Publishing message {i + 1}/{options.Count}...");
                    
                    var data = new SensorData(options.SensorId, options.PayloadSize);
                    var publishTime = await publisher.PublishAsync(data);
                    
                    publishTimes.Add(publishTime);
                    Console.WriteLine($"  Publish time: {publishTime * 1000:F2}ms");
                    
                    if (i < options.Count - 1)
                    {
                        await Task.Delay(TimeSpan.FromSeconds(options.Interval));
                    }
                }

                Console.WriteLine();
                Console.WriteLine($"✓ Published {options.Count} messages");
                
                if (publishTimes.Count > 0)
                {
                    var avgTime = publishTimes.Average();
                    Console.WriteLine($"✓ Average publish time: {avgTime * 1000:F2}ms");
                }

                await publisher.DisconnectAsync();
            }
            catch (Exception e)
            {
                Console.WriteLine($"✗ Error: {e.Message}");
                Environment.Exit(1);
            }
        }

        private static PublisherOptions ParseArgs(string[] args)
        {
            var options = new PublisherOptions();
            
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
                    case "--sensor-id" when i + 1 < args.Length:
                        options.SensorId = args[++i];
                        break;
                    case "--count" when i + 1 < args.Length:
                        options.Count = int.Parse(args[++i]);
                        break;
                    case "--interval" when i + 1 < args.Length:
                        options.Interval = double.Parse(args[++i]);
                        break;
                    case "--payload" when i + 1 < args.Length:
                        options.PayloadSize = args[++i];
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

    public class PublisherOptions
    {
        public string Broker { get; set; } = "localhost";
        public int Port { get; set; } = 1883;
        public string Topic { get; set; } = "mqtt-demo/all";
        public string SensorId { get; set; } = "sensor_001";
        public int Count { get; set; } = 10;
        public double Interval { get; set; } = 1.0;
        public string PayloadSize { get; set; } = "small";
        public int Qos { get; set; } = 1;
        public string Encoding { get; set; } = "json";
    }
}

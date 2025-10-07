using System.Text.Json;
using System.Text.Json.Serialization;
using MessagePack;

namespace MQTTComparison
{
    [MessagePackObject]
    public class Location
    {
        [Key(0)]
        public double Lat { get; set; }
        [Key(1)]
        public double Lon { get; set; }
        [Key(2)]
        public double Altitude { get; set; }
    }

    [MessagePackObject]
    public class Metadata
    {
        [Key(0)]
        public string FirmwareVersion { get; set; } = string.Empty;
        [Key(1)]
        public string HardwareId { get; set; } = string.Empty;
        [Key(2)]
        public string CalibrationDate { get; set; } = string.Empty;
        [Key(3)]
        public string LastMaintenance { get; set; } = string.Empty;
    }

    [MessagePackObject]
    public class SensorData
    {
        [Key(0)]
        public double Timestamp { get; set; }
        [Key(1)]
        public string SensorId { get; set; } = string.Empty;
        [Key(2)]
        public double Temperature { get; set; }
        [Key(3)]
        public double Humidity { get; set; }
        [Key(4)]
        public double Pressure { get; set; }
        
        // Optional fields for larger payloads
        [Key(5)]
        public Location? Location { get; set; }
        [Key(6)]
        public string? Status { get; set; }
        [Key(7)]
        public double? BatteryLevel { get; set; }
        [Key(8)]
        public int? SignalStrength { get; set; }
        [Key(9)]
        public List<double>? SensorReadings { get; set; }
        [Key(10)]
        public Metadata? Metadata { get; set; }
        [Key(11)]
        public string? AdditionalData { get; set; }

        public SensorData(string sensorId, string payloadSize = "small")
        {
            // Set timestamp
            Timestamp = DateTimeOffset.UtcNow.ToUnixTimeSeconds();
            SensorId = sensorId;
            
            // Generate random values
            var random = new Random();
            Temperature = 20.0 + random.NextDouble() * 20.0 - 5.0;
            Humidity = 30.0 + random.NextDouble() * 40.0;
            Pressure = 1000.0 + random.NextDouble() * 100.0 - 50.0;
            
            // Add additional fields based on payload size
            if (payloadSize == "medium")
            {
                Location = new Location { Lat = 40.7128, Lon = -74.0060, Altitude = 10.5 };
                Status = "active";
                BatteryLevel = 85.5;
                SignalStrength = -65;
                AdditionalData = new string('x', 1500);
            }
            else if (payloadSize == "large")
            {
                Location = new Location { Lat = 40.7128, Lon = -74.0060, Altitude = 10.5 };
                Status = "active";
                BatteryLevel = 85.5;
                SignalStrength = -65;
                SensorReadings = Enumerable.Range(0, 100).Select(_ => random.NextDouble() * 100.0).ToList();
                Metadata = new Metadata
                {
                    FirmwareVersion = "1.2.3",
                    HardwareId = "HW-001",
                    CalibrationDate = "2024-01-01",
                    LastMaintenance = "2024-06-01"
                };
                AdditionalData = new string('x', 60000);
            }
        }

        public string ToJson()
        {
            var options = new JsonSerializerOptions
            {
                PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
                WriteIndented = false
            };
            return JsonSerializer.Serialize(this, options);
        }

        public static SensorData FromJson(string json)
        {
            var options = new JsonSerializerOptions
            {
                PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
                WriteIndented = false
            };
            return JsonSerializer.Deserialize<SensorData>(json, options) ?? new SensorData("unknown");
        }

        public long GetSize()
        {
            return ToJson().Length;
        }
    }
}

package com.mqttcomparison;

import com.fasterxml.jackson.annotation.JsonProperty;
import java.time.Instant;
import java.util.List;
import java.util.Random;

/**
 * Sensor data model for MQTT comparison project.
 */
public class SensorData {
    @JsonProperty("timestamp")
    private double timestamp;
    
    @JsonProperty("sensor_id")
    private String sensorId;
    
    @JsonProperty("temperature")
    private double temperature;
    
    @JsonProperty("humidity")
    private double humidity;
    
    @JsonProperty("pressure")
    private double pressure;
    
    // Optional fields for larger payloads
    @JsonProperty("location")
    private Location location;
    
    @JsonProperty("status")
    private String status;
    
    @JsonProperty("battery_level")
    private Double batteryLevel;
    
    @JsonProperty("signal_strength")
    private Integer signalStrength;
    
    @JsonProperty("sensor_readings")
    private List<Double> sensorReadings;
    
    @JsonProperty("metadata")
    private Metadata metadata;
    
    @JsonProperty("additional_data")
    private String additionalData;
    
    // Default constructor
    public SensorData() {}
    
    // Constructor with sensor ID and payload size
    public SensorData(String sensorId, String payloadSize) {
        this.timestamp = Instant.now().toEpochMilli() / 1000.0;
        this.sensorId = sensorId;
        
        // Generate random values
        Random random = new Random();
        this.temperature = 20.0 + random.nextDouble() * 20.0 - 5.0;
        this.humidity = 30.0 + random.nextDouble() * 40.0;
        this.pressure = 1000.0 + random.nextDouble() * 100.0 - 50.0;
        
        // Add additional fields based on payload size
        if ("medium".equals(payloadSize)) {
            this.location = new Location(40.7128, -74.0060, 10.5);
            this.status = "active";
            this.batteryLevel = 85.5;
            this.signalStrength = -65;
            this.additionalData = "x".repeat(1500);
        } else if ("large".equals(payloadSize)) {
            this.location = new Location(40.7128, -74.0060, 10.5);
            this.status = "active";
            this.batteryLevel = 85.5;
            this.signalStrength = -65;
            this.sensorReadings = random.doubles(100, 0, 100).boxed().toList();
            this.metadata = new Metadata("1.2.3", "HW-001", "2024-01-01", "2024-06-01");
            this.additionalData = "x".repeat(60000);
        }
    }
    
    // Getters and setters
    public double getTimestamp() { return timestamp; }
    public void setTimestamp(double timestamp) { this.timestamp = timestamp; }
    
    public String getSensorId() { return sensorId; }
    public void setSensorId(String sensorId) { this.sensorId = sensorId; }
    
    public double getTemperature() { return temperature; }
    public void setTemperature(double temperature) { this.temperature = temperature; }
    
    public double getHumidity() { return humidity; }
    public void setHumidity(double humidity) { this.humidity = humidity; }
    
    public double getPressure() { return pressure; }
    public void setPressure(double pressure) { this.pressure = pressure; }
    
    public Location getLocation() { return location; }
    public void setLocation(Location location) { this.location = location; }
    
    public String getStatus() { return status; }
    public void setStatus(String status) { this.status = status; }
    
    public Double getBatteryLevel() { return batteryLevel; }
    public void setBatteryLevel(Double batteryLevel) { this.batteryLevel = batteryLevel; }
    
    public Integer getSignalStrength() { return signalStrength; }
    public void setSignalStrength(Integer signalStrength) { this.signalStrength = signalStrength; }
    
    public List<Double> getSensorReadings() { return sensorReadings; }
    public void setSensorReadings(List<Double> sensorReadings) { this.sensorReadings = sensorReadings; }
    
    public Metadata getMetadata() { return metadata; }
    public void setMetadata(Metadata metadata) { this.metadata = metadata; }
    
    public String getAdditionalData() { return additionalData; }
    public void setAdditionalData(String additionalData) { this.additionalData = additionalData; }
    
    // Nested classes
    public static class Location {
        @JsonProperty("lat")
        private double lat;
        
        @JsonProperty("lon")
        private double lon;
        
        @JsonProperty("altitude")
        private double altitude;
        
        public Location() {}
        
        public Location(double lat, double lon, double altitude) {
            this.lat = lat;
            this.lon = lon;
            this.altitude = altitude;
        }
        
        // Getters and setters
        public double getLat() { return lat; }
        public void setLat(double lat) { this.lat = lat; }
        
        public double getLon() { return lon; }
        public void setLon(double lon) { this.lon = lon; }
        
        public double getAltitude() { return altitude; }
        public void setAltitude(double altitude) { this.altitude = altitude; }
    }
    
    public static class Metadata {
        @JsonProperty("firmware_version")
        private String firmwareVersion;
        
        @JsonProperty("hardware_id")
        private String hardwareId;
        
        @JsonProperty("calibration_date")
        private String calibrationDate;
        
        @JsonProperty("last_maintenance")
        private String lastMaintenance;
        
        public Metadata() {}
        
        public Metadata(String firmwareVersion, String hardwareId, String calibrationDate, String lastMaintenance) {
            this.firmwareVersion = firmwareVersion;
            this.hardwareId = hardwareId;
            this.calibrationDate = calibrationDate;
            this.lastMaintenance = lastMaintenance;
        }
        
        // Getters and setters
        public String getFirmwareVersion() { return firmwareVersion; }
        public void setFirmwareVersion(String firmwareVersion) { this.firmwareVersion = firmwareVersion; }
        
        public String getHardwareId() { return hardwareId; }
        public void setHardwareId(String hardwareId) { this.hardwareId = hardwareId; }
        
        public String getCalibrationDate() { return calibrationDate; }
        public void setCalibrationDate(String calibrationDate) { this.calibrationDate = calibrationDate; }
        
        public String getLastMaintenance() { return lastMaintenance; }
        public void setLastMaintenance(String lastMaintenance) { this.lastMaintenance = lastMaintenance; }
    }
}

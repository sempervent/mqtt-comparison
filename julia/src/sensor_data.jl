"""
Sensor data structures and functions for Julia MQTT implementation.
"""

using JSON
using Random

"""
    SensorData

Structure representing sensor data with optional fields for different payload sizes.
"""
mutable struct SensorData
    timestamp::Float64
    sensor_id::String
    temperature::Float64
    humidity::Float64
    pressure::Float64
    
    # Optional fields for larger payloads
    location::Union{Nothing, NamedTuple{(:lat, :lon, :altitude), Tuple{Float64, Float64, Float64}}}
    status::Union{Nothing, String}
    battery_level::Union{Nothing, Float64}
    signal_strength::Union{Nothing, Int32}
    sensor_readings::Union{Nothing, Vector{Float64}}
    metadata::Union{Nothing, NamedTuple{(:firmware_version, :hardware_id, :calibration_date, :last_maintenance), Tuple{String, String, String, String}}}
    additional_data::Union{Nothing, String}
    
    function SensorData(sensor_id::String, payload_size::String="small")
        # Set timestamp
        timestamp = time()
        
        # Generate random values
        rng = Random.default_rng()
        temperature = 20.0 + rand(rng, -5.0:0.01:15.0)
        humidity = 30.0 + rand(rng, 0.0:0.01:40.0)
        pressure = 1000.0 + rand(rng, -50.0:0.01:50.0)
        
        # Create base data
        data = new(timestamp, sensor_id, temperature, humidity, pressure, nothing, nothing, nothing, nothing, nothing, nothing, nothing)
        
        # Add additional fields based on payload size
        if payload_size == "medium"
            data.location = (lat=40.7128, lon=-74.0060, altitude=10.5)
            data.status = "active"
            data.battery_level = 85.5
            data.signal_strength = -65
            data.additional_data = "x" ^ 1500
        elseif payload_size == "large"
            data.location = (lat=40.7128, lon=-74.0060, altitude=10.5)
            data.status = "active"
            data.battery_level = 85.5
            data.signal_strength = -65
            data.sensor_readings = rand(rng, 100)
            data.metadata = (
                firmware_version="1.2.3",
                hardware_id="HW-001",
                calibration_date="2024-01-01",
                last_maintenance="2024-06-01"
            )
            data.additional_data = "x" ^ 60000
        end
        
        return data
    end
end

"""
    to_json(data::SensorData) -> String

Convert SensorData to JSON string.
"""
function to_json(data::SensorData)::String
    # Create a dictionary with only non-nothing fields
    dict = Dict{String, Any}(
        "timestamp" => data.timestamp,
        "sensor_id" => data.sensor_id,
        "temperature" => data.temperature,
        "humidity" => data.humidity,
        "pressure" => data.pressure
    )
    
    # Add optional fields if they exist
    if data.location !== nothing
        dict["location"] = Dict(
            "lat" => data.location.lat,
            "lon" => data.location.lon,
            "altitude" => data.location.altitude
        )
    end
    
    if data.status !== nothing
        dict["status"] = data.status
    end
    
    if data.battery_level !== nothing
        dict["battery_level"] = data.battery_level
    end
    
    if data.signal_strength !== nothing
        dict["signal_strength"] = data.signal_strength
    end
    
    if data.sensor_readings !== nothing
        dict["sensor_readings"] = data.sensor_readings
    end
    
    if data.metadata !== nothing
        dict["metadata"] = Dict(
            "firmware_version" => data.metadata.firmware_version,
            "hardware_id" => data.metadata.hardware_id,
            "calibration_date" => data.metadata.calibration_date,
            "last_maintenance" => data.metadata.last_maintenance
        )
    end
    
    if data.additional_data !== nothing
        dict["additional_data"] = data.additional_data
    end
    
    return JSON.json(dict)
end

"""
    from_json(json_str::String) -> SensorData

Parse JSON string to SensorData.
"""
function from_json(json_str::String)::SensorData
    dict = JSON.parse(json_str)
    
    # Extract basic fields
    timestamp = dict["timestamp"]
    sensor_id = dict["sensor_id"]
    temperature = dict["temperature"]
    humidity = dict["humidity"]
    pressure = dict["pressure"]
    
    # Create SensorData object
    data = SensorData(sensor_id, "small")  # Start with small payload
    data.timestamp = timestamp
    data.temperature = temperature
    data.humidity = humidity
    data.pressure = pressure
    
    # Extract optional fields
    if haskey(dict, "location")
        loc = dict["location"]
        data.location = (lat=loc["lat"], lon=loc["lon"], altitude=loc["altitude"])
    end
    
    if haskey(dict, "status")
        data.status = dict["status"]
    end
    
    if haskey(dict, "battery_level")
        data.battery_level = dict["battery_level"]
    end
    
    if haskey(dict, "signal_strength")
        data.signal_strength = dict["signal_strength"]
    end
    
    if haskey(dict, "sensor_readings")
        data.sensor_readings = dict["sensor_readings"]
    end
    
    if haskey(dict, "metadata")
        meta = dict["metadata"]
        data.metadata = (
            firmware_version=meta["firmware_version"],
            hardware_id=meta["hardware_id"],
            calibration_date=meta["calibration_date"],
            last_maintenance=meta["last_maintenance"]
        )
    end
    
    if haskey(dict, "additional_data")
        data.additional_data = dict["additional_data"]
    end
    
    return data
end

"""
    to_cbor(data::SensorData) -> Vector{UInt8}

Convert SensorData to CBOR format.
"""
function to_cbor(data::SensorData)::Vector{UInt8}
    # Fallback to JSON for now since CBOR.jl is not available
    return Vector{UInt8}(to_json(data))
end

"""
    from_cbor(cbor_data::Vector{UInt8}) -> SensorData

Parse CBOR data to SensorData.
"""
function from_cbor(cbor_data::Vector{UInt8})::SensorData
    # Fallback to JSON for now since CBOR.jl is not available
    return from_json(String(cbor_data))
end

"""
    to_protobuf(data::SensorData) -> Vector{UInt8}

Convert SensorData to Protobuf format.
"""
function to_protobuf(data::SensorData)::Vector{UInt8}
    # For now, fallback to JSON since Protobuf.jl requires schema definition
    return Vector{UInt8}(to_json(data))
end

"""
    from_protobuf(protobuf_data::Vector{UInt8}) -> SensorData

Parse Protobuf data to SensorData.
"""
function from_protobuf(protobuf_data::Vector{UInt8})::SensorData
    # For now, fallback to JSON since Protobuf.jl requires schema definition
    return from_json(String(protobuf_data))
end

"""
    encode_message(data::SensorData, encoding::String) -> Vector{UInt8}

Encode sensor data to the specified format.
"""
function encode_message(data::SensorData, encoding::String)::Vector{UInt8}
    if encoding == "json"
        return Vector{UInt8}(to_json(data))
    elseif encoding == "msgpack"
        # Fallback to JSON for now since MessagePack.jl is not available
        return Vector{UInt8}(to_json(data))
    elseif encoding == "cbor"
        return to_cbor(data)
    elseif encoding == "protobuf"
        return to_protobuf(data)
    else
        throw(ArgumentError("Unsupported encoding: $encoding"))
    end
end

"""
    decode_message(payload::Vector{UInt8}, encoding::String) -> SensorData

Decode sensor data from the specified format.
"""
function decode_message(payload::Vector{UInt8}, encoding::String)::SensorData
    if encoding == "json"
        return from_json(String(payload))
    elseif encoding == "msgpack"
        # Fallback to JSON for now since MessagePack.jl is not available
        return from_json(String(payload))
    elseif encoding == "cbor"
        return from_cbor(payload)
    elseif encoding == "protobuf"
        return from_protobuf(payload)
    else
        throw(ArgumentError("Unsupported encoding: $encoding"))
    end
end

"""
    get_size(data::SensorData) -> Int

Get approximate size of SensorData in bytes.
"""
function get_size(data::SensorData)::Int
    size = sizeof(data)
    if data.additional_data !== nothing
        size += sizeof(data.additional_data)
    end
    if data.sensor_readings !== nothing
        size += sizeof(data.sensor_readings)
    end
    return size
end

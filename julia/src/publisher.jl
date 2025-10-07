"""
MQTT Publisher for Julia with JSON encoding support.
"""

using ArgParse
using .SensorData
using .MQTTClient

function parse_args()
    s = ArgParseSettings()
    
    @add_arg_table! s begin
        "--broker"
            help = "MQTT broker hostname"
            arg_type = String
            default = "localhost"
        "--port"
            help = "MQTT broker port"
            arg_type = Int
            default = 1883
        "--topic"
            help = "MQTT topic"
            arg_type = String
            default = "mqtt-demo/all"
        "--sensor-id"
            help = "Sensor identifier"
            arg_type = String
            default = "sensor_001"
        "--count"
            help = "Number of messages to publish"
            arg_type = Int
            default = 10
        "--interval"
            help = "Interval between messages in seconds"
            arg_type = Float64
            default = 1.0
        "--payload"
            help = "Payload size variant"
            arg_type = String
            default = "small"
        "--qos"
            help = "Quality of Service level"
            arg_type = Int
            default = 1
        "--encoding"
            help = "Encoding format"
            arg_type = String
            default = "json"
    end
    
    return parse_args(s)
end

function main()
    args = parse_args()
    
    println("=== MQTT Publisher (Julia) ===")
    println("Broker: $(args["broker"]):$(args["port"])")
    println("Topic: $(args["topic"])")
    println("Payload: $(args["payload"])")
    println("QoS: $(args["qos"])")
    println("Encoding: $(args["encoding"])")
    println()
    
    try
        # Create MQTT client
        client = MQTTClient(args["broker"], args["port"])
        
        # Connect to broker
        connect(client)
        println("✓ Connected to $(args["broker"]):$(args["port"])")
        
        # Publish messages
        publish_times = Float64[]
        
        for i in 1:args["count"]
            println("Publishing message $i/$(args["count"])...")
            
            # Create sensor data
            data = SensorData(args["sensor_id"], args["payload"])
            
            # Measure publish time
            start_time = time()
            
            # Encode message
            payload = encode_message(data, args["encoding"])
            
            # Publish message
            publish(client, args["topic"], payload, qos=args["qos"])
            
            end_time = time()
            publish_time = end_time - start_time
            push!(publish_times, publish_time)
            
            println("  Publish time: $(round(publish_time * 1000, digits=2))ms")
            
            if i < args["count"]
                sleep(args["interval"])
            end
        end
        
        println()
        println("✓ Published $(args["count"]) messages")
        
        if !isempty(publish_times)
            avg_time = sum(publish_times) / length(publish_times)
            println("✓ Average publish time: $(round(avg_time * 1000, digits=2))ms")
        end
        
        # Disconnect
        disconnect(client)
        println("✓ Disconnected")
        
    catch e
        println("✗ Error: $e")
        exit(1)
    end
end

if abspath(PROGRAM_FILE) == @__FILE__
    main()
end

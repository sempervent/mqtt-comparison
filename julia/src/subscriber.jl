"""
MQTT Subscriber for Julia with JSON encoding support.
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
    
    println("=== MQTT Subscriber (Julia) ===")
    println("Broker: $(args["broker"]):$(args["port"])")
    println("Topic: $(args["topic"])")
    println("QoS: $(args["qos"])")
    println("Encoding: $(args["encoding"])")
    println()
    
    message_count = 0
    receive_times = Float64[]
    
    try
        # Create MQTT client
        client = MQTTClient(args["broker"], args["port"])
        
        # Set up message callback
        function message_callback(topic::String, payload::String)
            receive_time = time()
            message_count += 1
            
            try
                # Decode message
                data = decode_message(Vector{UInt8}(payload), args["encoding"])
                
                println("\n[Message $message_count] Topic: $topic")
                println("  Sensor ID: $(data.sensor_id)")
                println("  Temperature: $(round(data.temperature, digits=2))°C")
                println("  Humidity: $(round(data.humidity, digits=2))%")
                println("  Pressure: $(round(data.pressure, digits=2)) hPa")
                println("  Timestamp: $(round(data.timestamp, digits=6))")
                
                # Calculate receive latency
                latency = receive_time - data.timestamp
                push!(receive_times, latency)
                println("  Receive latency: $(round(latency * 1000, digits=2))ms")
                
            catch e
                println("\n[Message $message_count] Failed to decode message: $e")
            end
        end
        
        # Connect to broker
        connect(client)
        println("✓ Connected to $(args["broker"]):$(args["port"])")
        
        # Subscribe to topic
        subscribe(client, args["topic"], message_callback, qos=args["qos"])
        println("✓ Subscribed to topic: $(args["topic"]) (QoS: $(args["qos"]))")
        println("\nWaiting for messages (Ctrl+C to exit)...\n")
        
        # Keep running until interrupted
        while true
            sleep(0.1)
        end
        
    catch InterruptException
        println("\n\n✓ Received $message_count messages")
        
        if !isempty(receive_times)
            avg_latency = sum(receive_times) / length(receive_times)
            println("✓ Average receive latency: $(round(avg_latency * 1000, digits=2))ms")
        end
        
        println("✓ Disconnected")
        
    catch e
        println("✗ Error: $e")
        exit(1)
    end
end

if abspath(PROGRAM_FILE) == @__FILE__
    main()
end

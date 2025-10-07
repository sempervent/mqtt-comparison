cmake_minimum_required(VERSION 3.10)
project(mqtt_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

# Include directories
include_directories(/usr/include)
include_directories(/usr/local/include)

# Link directories
link_directories(/usr/lib)
link_directories(/usr/local/lib)

# Source files
set(PUBLISHER_SOURCES
    src/publisher.cpp
    src/sensor_data.cpp
    src/json_encoder.cpp
    src/sensor_data.pb.cc
)

set(SUBSCRIBER_SOURCES
    src/subscriber.cpp
    src/sensor_data.cpp
    src/json_encoder.cpp
    src/sensor_data.pb.cc
)

# Create executables
add_executable(publisher ${PUBLISHER_SOURCES})
add_executable(subscriber ${SUBSCRIBER_SOURCES})

# Link libraries
target_link_libraries(publisher paho-mqttpp3 paho-mqtt3as jsoncpp msgpackc cbor protobuf)
target_link_libraries(subscriber paho-mqttpp3 paho-mqtt3as jsoncpp msgpackc cbor protobuf)

# Set output directory
set_target_properties(publisher PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(subscriber PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Compiler flags
target_compile_options(publisher PRIVATE -Wall -Wextra -O2)
target_compile_options(subscriber PRIVATE -Wall -Wextra -O2)

# Installation
install(TARGETS publisher subscriber
    RUNTIME DESTINATION bin
)

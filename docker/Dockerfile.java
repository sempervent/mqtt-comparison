# Java MQTT Implementation Dockerfile
FROM openjdk:17-jdk-slim

WORKDIR /app

# Install Maven
RUN apt-get update && apt-get install -y maven && rm -rf /var/lib/apt/lists/*

# Copy Java project files
COPY java/pom.xml .
COPY java/src/ ./src/

# Build the project
RUN mvn clean package -DskipTests

# Copy schemas
COPY schemas/ ./schemas/

# Create results directory
RUN mkdir -p results

# Set default command
CMD ["java", "-jar", "target/mqtt-java-1.0.0.jar", "publisher"]

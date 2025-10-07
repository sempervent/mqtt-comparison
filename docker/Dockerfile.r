# R MQTT Implementation Dockerfile
FROM r-base:latest

WORKDIR /app

# Install system dependencies
RUN apt-get update && apt-get install -y \
    libcurl4-openssl-dev \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy R source code
COPY r/ ./src/
COPY schemas/ ./schemas/

# Install R packages
RUN R -e "install.packages(c('mqtt', 'jsonlite', 'msgpack', 'argparse'), repos='https://cran.r-project.org')"

# Create results directory
RUN mkdir -p results

# Set default command
CMD ["Rscript", "src/publisher.R"]

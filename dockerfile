FROM ubuntu:latest

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    valgrind \
    pkg-config \
    libjansson-dev \
    libmicrohttpd-dev \
    libpg-query-dev \
    && rm -rf /var/lib/apt/lists/*

# Set up working directory
WORKDIR /app

# Copy source code
COPY src/ /app/src/
COPY tests/ /app/tests/

# Create an internal Makefile for Linux environment
COPY dockerized.mk /app/Makefile

# Build the application
RUN make

# Set entrypoint to run the application
ENTRYPOINT ["/app/pg_migration_graph.out"]
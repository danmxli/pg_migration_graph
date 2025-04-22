FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    pkg-config \
    libjansson-dev \
    libmicrohttpd-dev \
    && rm -rf /var/lib/apt/lists/*

# Set up working directory
WORKDIR /app

# Copy source code
COPY src/ /app/src/
COPY tests/ /app/tests/

# Clone and build libpg_query
RUN mkdir -p deps && \
    cd deps && \
    git clone https://github.com/pganalyze/libpg_query.git && \
    cd libpg_query && \
    make

# Create an internal Makefile for Linux environment
COPY dockerized.mk /app/Makefile

# Build the application
RUN make

# Set entrypoint to run the application
ENTRYPOINT ["/app/pg_migration_graph.out"]
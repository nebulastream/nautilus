FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive

# Core build tools
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    ccache \
    git \
    curl \
    wget \
    gnupg \
    lsb-release \
    software-properties-common \
    pkg-config \
    # Stack trace support (backward-cpp)
    binutils-dev \
    libdw-dev \
    libdwarf-dev \
    # Misc
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Install Clang 21 toolchain from LLVM APT repos
RUN wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc && \
    echo "deb http://apt.llvm.org/noble/ llvm-toolchain-noble-21 main" | tee /etc/apt/sources.list.d/llvm-21.list && \
    apt-get update && \
    apt-get install -y \
    clang-21 \
    clang++-21 \
    clang-format-21 \
    clang-tidy-21 \
    llvm-21 \
    lld-21 \
    && rm -rf /var/lib/apt/lists/*

# Set Clang 21 as the default compiler
ENV CC=clang-21
ENV CXX=clang++-21

# Symlinks for convenience
RUN ln -sf /usr/bin/clang-21 /usr/local/bin/clang && \
    ln -sf /usr/bin/clang++-21 /usr/local/bin/clang++ && \
    ln -sf /usr/bin/clang-format-21 /usr/local/bin/clang-format && \
    ln -sf /usr/bin/clang-tidy-21 /usr/local/bin/clang-tidy && \
    ln -sf /usr/bin/llvm-diff-21 /usr/local/bin/llvm-diff-19 && \
    ln -sf /usr/bin/lld-21 /usr/local/bin/lld

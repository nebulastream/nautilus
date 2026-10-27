# The pinned CI image of the 0.4 measurement jobs (docs/plans/nautilus-0.4/16-implementation-plan.md, M1.11; D9): the
# latency-corpus and baselines-0.3 jobs run in it so that instruction counts (callgrind) are reproducible across runs.
# Build and publish it with the `ci-image` workflow, which prints the digest of the result; the jobs pin the image by that
# digest, not by a tag.
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        ca-certificates curl gnupg git cmake ninja-build ccache python3 valgrind gcc-14 g++-14 libffi-dev \
    && curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor -o /usr/share/keyrings/llvm.gpg \
    && echo "deb [signed-by=/usr/share/keyrings/llvm.gpg] https://apt.llvm.org/noble/ llvm-toolchain-noble-21 main" \
        > /etc/apt/sources.list.d/llvm.list \
    && apt-get update \
    && apt-get install -y --no-install-recommends clang-21 llvm-21-dev \
    && rm -rf /var/lib/apt/lists/*
ENV CC=clang-21 CXX=clang++-21

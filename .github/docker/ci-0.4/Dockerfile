# The pinned CI image of the 0.4 measurement jobs (docs/plans/nautilus-0.4/16-implementation-plan.md, M1.11; D9): the
# callgrind gate (`callgrind-gate` in pr.yml, tools/latency) and the latency-corpus and baselines-0.3 jobs run in it so
# that instruction counts are reproducible across runs.
#
# Everything the counts depend on is pinned, so that rebuilding the image gives the same toolchain and libraries:
#   - the base image by digest (the multi-architecture index of ubuntu:24.04);
#   - the Ubuntu packages by a dated snapshot of the archive (snapshot.ubuntu.com), not by the live archive: libc,
#     libstdc++, valgrind, CMake, GCC 14 and the rest are those of UBUNTU_SNAPSHOT;
#   - Clang 21 by its exact version from apt.llvm.org (the 21 branch is closed: 21.1.8 is its last release), and the
#     repository key by its checksum.
# The gate records a fingerprint of valgrind, libc, libstdc++ and the compiler (callgrind-gate.py): moving any of these
# changes it, and the job then fails until the thresholds are re-recorded in the new image (.github/docker/README.md).
#
# The job builds this image itself (docker build with the GitHub cache). The `ci-image` workflow also publishes it and
# prints its digest, which is what a published, named image would be pinned by once the repository owners do that.
FROM ubuntu:24.04@sha256:fe81560990fae86dc4743202b2cb4d3df98d0bcf949173cecef15ce1127ef852

ARG UBUNTU_SNAPSHOT=20261101T000000Z
ARG LLVM_VERSION=1:21.1.8~++20251221032922+2078da43e25a-1~exp1~20251221153059.70
ARG LLVM_KEY_SHA256=8b2a587ffd672c4687e7581dad4b2f6c1bb2ad6b480cd9771ba2ff48e0b8c75d

ENV DEBIAN_FRONTEND=noninteractive
# The snapshot redirects to https and the base image has no CA certificates yet: the first fetch skips the TLS check
# (apt still verifies the signed Release files against the archive keyring), then the certificates make it normal.
RUN rm -f /etc/apt/sources.list.d/ubuntu.sources /etc/apt/sources.list \
    && for pocket in noble noble-updates noble-security; do \
         echo "deb [signed-by=/usr/share/keyrings/ubuntu-archive-keyring.gpg] http://snapshot.ubuntu.com/ubuntu/${UBUNTU_SNAPSHOT}/ ${pocket} main restricted universe multiverse"; \
       done > /etc/apt/sources.list \
    && printf 'Acquire::Retries "5";\nAcquire::Check-Valid-Until "false";\n' > /etc/apt/apt.conf.d/99pinned \
    && apt-get update -o Acquire::https::Verify-Peer=false -o Acquire::https::Verify-Host=false \
    && apt-get install -y --no-install-recommends -o Acquire::https::Verify-Peer=false -o Acquire::https::Verify-Host=false \
        ca-certificates \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        curl gnupg git cmake ninja-build ccache python3 binutils util-linux valgrind libc6-dbg \
        gcc-14 g++-14 libffi-dev \
    && curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key -o /tmp/llvm.key \
    && echo "${LLVM_KEY_SHA256}  /tmp/llvm.key" | sha256sum -c - \
    && gpg --dearmor -o /usr/share/keyrings/llvm.gpg /tmp/llvm.key \
    && rm /tmp/llvm.key \
    && echo "deb [signed-by=/usr/share/keyrings/llvm.gpg] https://apt.llvm.org/noble/ llvm-toolchain-noble-21 main" \
        > /etc/apt/sources.list.d/llvm.list \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        "clang-21=${LLVM_VERSION}" "llvm-21-dev=${LLVM_VERSION}" \
    && rm -rf /var/lib/apt/lists/*
ENV CC=clang-21 CXX=clang++-21 CCACHE_DIR=/ccache
